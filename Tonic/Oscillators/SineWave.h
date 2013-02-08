//
//  SineWave.h
//  2013_1_23_melody
//
//  Created by Morgan Packard on 1/23/13.
//
//

/*+++++++++++++++++++++ License ++++++++++++++++++++

Use this code for whatever you want. There are NO 
RESTRICTIONS WHATSOVER. Modify it, repackage it, 
sell it, get rich from it, whatever. Go crazy. If 
you want to make mehappy, contribute to this 
project, but feel free to just use the code as a 
starting point for whatever you like.

Note that Tonic is heavily indebted to STK
https://ccrma.stanford.edu/software/stk/

++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
  For now, this is a line-for-line copy of STK SineWave_. Much thanks to Perry Cook and Gary Scavone. 
  https://ccrma.stanford.edu/software/stk/
*/

#ifndef ___013_1_23_melody__SineWave___
#define ___013_1_23_melody__SineWave___

#include <iostream>
#include "Generator.h"
#include "FixedValue.h"

namespace Tonic {

  namespace Tonic_{

    const unsigned long TABLE_SIZE = 2048;

    class SineWave_ : public Generator_
    {
    public:
      //! Default constructor.
      SineWave_( void );

      //! Class destructor.
      ~SineWave_( void );

      //! Clear output and reset time pointer to zero.
      void reset( void );

      //! Set the data read rate in samples.  The rate can be negative.
      /*!
        If the rate value is negative, the data is read in reverse order.
      */
      inline void setRate( TonicFloat rate ) { rate_ = rate; };

      //! Set the data interpolation rate based on a looping frequency.
      /*!
        This function determines the interpolation rate based on the file
        size and the current Stk::sampleRate.  The \e frequency value
        corresponds to file cycles per second.  The frequency can be
        negative, in which case the loop is read in reverse order.
       */
      void setFrequency( TonicFloat frequency );

      //! Increment the read pointer by \e time in samples, modulo the table size.
      void addTime( TonicFloat time );

      //! Increment the read pointer by a normalized \e phase value.
      /*!
        This function increments the read pointer by a normalized phase
        value, such that \e phase = 1.0 corresponds to a 360 degree phase
        shift.  Positive or negative values are possible.
       */
      void addPhase( TonicFloat phase );

      //! Add a normalized phase offset to the read pointer.
      /*!
        A \e phaseOffset = 1.0 corresponds to a 360 degree phase
        offset.  Positive or negative values are possible.
       */
      void addPhaseOffset( TonicFloat phaseOffset );

      //! Compute and return one output sample.
      TonicFloat tick( void );

      //! Fill a channel of the TonicFrames object with computed outputs.
      /*!
        The \c channel argument must be less than the number of
        channels in the TonicFrames argument (the first channel is specified
        by 0).  However, range checking is only performed if _STK_DEBUG_
        is defined during compilation, in which case an out-of-range value
        will trigger an StkError exception.
      */
      inline void tick( TonicFrames& frames );

    protected:
  //
  //    void sampleRateChanged( TonicFloat newRate, TonicFloat oldRate );

      static TonicFrames table_;
      TonicFloat time_;
      TonicFloat rate_;
      TonicFloat phaseOffset_;
      unsigned int iIndex_;
      TonicFloat alpha_;

    };

    inline void SineWave_ :: tick( TonicFrames& frames )
    {
    #if defined(_STK_DEBUG_)
      if ( channel >= frames.channels() ) {
        oStream_ << "SineWave_::tick(): channel and TonicFrames arguments are incompatible!";
        handleError( StkError::FUNCTION_ARGUMENT );
      }
    #endif

      TonicFloat *samples = &frames[0];
      TonicFloat tmp = 0.0;

      unsigned int hop = frames.channels();
      for ( unsigned int i=0; i<frames.frames(); i++) {

        // Check limits of time address ... if necessary, recalculate modulo
        // TABLE_SIZE.
        while ( time_ < 0.0 )
          time_ += TABLE_SIZE;
        while ( time_ >= TABLE_SIZE )
          time_ -= TABLE_SIZE;

        iIndex_ = (unsigned int) time_;
        alpha_ = time_ - iIndex_;
        tmp = table_[ iIndex_ ];
        tmp += ( alpha_ * ( table_[ iIndex_ + 1 ] - tmp ) );
          
        // fill all channels of the interleaved output
        // it's up to the caller to request the right number of channels, since a sine wave is always mono
        for (unsigned int c=0; c<hop; c++){
          *samples++ = tmp;
        }

        // Increment time, which can be negative.
        time_ += rate_;
      }

    }
    
    class SineWaveMod_ : public SineWave_{
    
      protected:
          
          Generator frequencyGenerator;
          TonicFrames modFrames;
          
      public:
          
          SineWaveMod_(){
              modFrames.resize(kSynthesisBlockSize, 1);
          }
          
          virtual void tick( TonicFrames& frames );
          
          void setFrequency(Generator genArg){
              // need to lock mutex so we don't replace this in the middle of a tick
              lockMutex();
              frequencyGenerator = genArg;
              unlockMutex();
          }
      };
    
        
    inline void SineWaveMod_ :: tick( TonicFrames& frames )
    {
    #if defined(_STK_DEBUG_)
        if ( channel >= frames.channels() ) {
            errorString_ << "SineWave::tick(): channel and StkFrames arguments are incompatible!";
            handleError( StkError::FUNCTION_ARGUMENT );
        }
    #endif
            
        lockMutex();
        frequencyGenerator.tick(modFrames);
        unlockMutex();
      
        TonicFloat tmp = 0.0;
        TonicFloat *samples = &frames[0];
        TonicFloat *freqBuffer = &modFrames[0];
        TonicFloat *tableData = &table_[0];
      
        // pre-multiply rate constant
        static const TonicFloat rateConstant = (TonicFloat)TABLE_SIZE / Tonic::sampleRate();
        #ifdef USE_APPLE_ACCELERATE
        vDSP_vsmul(freqBuffer, 1, &rateConstant, freqBuffer, 1, modFrames.frames());
        #else
        for (unsigned int i=0; i<modFrames.frames(); i++){
          *freqBuffer++ *= rateConstant;
        }
        freqBuffer = &modFrames[0];
        #endif
      
        unsigned int hop = frames.channels();
        
        for ( unsigned int i=0; i<frames.frames(); i++ ) {
          
            // This can be optimized by pre-multiplying to get rate
            // SineWave_::setFrequency(*(freqBuffer++));
                    
            // Check limits of time address ... if necessary, recalculate modulo
            // TABLE_SIZE.
            while ( time_ < 0.0 )
                time_ += TABLE_SIZE;
            while ( time_ >= TABLE_SIZE )
                time_ -= TABLE_SIZE;
            
            iIndex_ = (unsigned int) time_;
            alpha_ = time_ - (TonicFloat)iIndex_;
            tmp = tableData[ iIndex_ ];
            tmp += ( alpha_ * ( tableData[ iIndex_ + 1 ] - tmp ) );
            
            // fill all channels of the interleaved output
            // it's up to the caller to request the right number of channels, since a sine wave is always mono
            *samples = tmp;
            samples += hop;
            
            // Increment time, which can be negative.
            // Directly add the rate, don't need to dive into function call
            time_ += *freqBuffer++;
        }
        
        // mono source, so copy channels if necessary
        frames.fillChannels();
    }

  
  }

  class SineWave : public TemplatedGenerator<Tonic_::SineWaveMod_>{
  public:
    
    SineWave& freq(Generator freq){
      gen()->setFrequency(freq);
      return *this;
    }
    
    SineWave& freq(float freqArg){
      return freq( FixedValue(freqArg) );
    }
    
  };

}

#endif /* defined(___013_1_23_melody__SineWave___) */
