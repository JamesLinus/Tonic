//
//  TonicFrames.h
//  2013_1_23_melody
//
//  Created by Morgan Packard on 1/23/13.
//
//

#ifndef ___013_1_23_melody__TonicFrames__
#define ___013_1_23_melody__TonicFrames__

#include <iostream>
#include "Tonic.h"

/*
  This is an almost exact copy of STKFrames. Many thanks to Perry Cook and Gary Scavone.
  https://ccrma.stanford.edu/software/stk/
*/

namespace Tonic {


  class TonicFrames
  {
  public:

    //! The default constructor initializes the frame data structure to size zero.
    TonicFrames( unsigned int nFrames = 0, unsigned int nChannels = 0 );

    //! Overloaded constructor that initializes the frame data to the specified size with \c value.
    TonicFrames( const TonicFloat& value, unsigned int nFrames, unsigned int nChannels );

    //! The destructor.
    virtual ~TonicFrames();

    // A copy constructor.
    TonicFrames( const TonicFrames& f );

    // Assignment operator that returns a reference to self.
    TonicFrames& operator= ( const TonicFrames& f );

    //! Subscript operator that returns a reference to element \c n of self.
    /*!
      The result can be used as an lvalue. This reference is valid
      until the resize function is called or the array is destroyed. The
      index \c n must be between 0 and size less one.  No range checking
      is performed unless _STK_DEBUG_ is defined.
    */
    TonicFloat& operator[] ( size_t n );

    //! Subscript operator that returns the value at element \c n of self.
    /*!
      The index \c n must be between 0 and size less one.  No range
      checking is performed unless _STK_DEBUG_ is defined.
    */
    TonicFloat operator[] ( size_t n ) const;

    //! Assignment by sum operator into self.
    /*!
      The dimensions of the argument are expected to be the same as
      self.  No range checking is performed unless _STK_DEBUG_ is
      defined.
    */
    void operator+= ( TonicFrames& f );

    //! Assignment by product operator into self.
    /*!
      The dimensions of the argument are expected to be the same as
      self.  No range checking is performed unless _STK_DEBUG_ is
      defined.
    */
    void operator*= ( TonicFrames& f );

    //! Channel / frame subscript operator that returns a reference.
    /*!
      The result can be used as an lvalue. This reference is valid
      until the resize function is called or the array is destroyed. The
      \c frame index must be between 0 and frames() - 1.  The \c channel
      index must be between 0 and channels() - 1.  No range checking is
      performed unless _STK_DEBUG_ is defined.
    */
    TonicFloat& operator() ( size_t frame, unsigned int channel );

    //! Channel / frame subscript operator that returns a value.
    /*!
      The \c frame index must be between 0 and frames() - 1.  The \c
      channel index must be between 0 and channels() - 1.  No range checking
      is performed unless _STK_DEBUG_ is defined.
    */
    TonicFloat operator() ( size_t frame, unsigned int channel ) const;

    //! Return an interpolated value at the fractional frame index and channel.
    /*!
      This function performs linear interpolation.  The \c frame
      index must be between 0.0 and frames() - 1.  The \c channel index
      must be between 0 and channels() - 1.  No range checking is
      performed unless _STK_DEBUG_ is defined.
    */
    TonicFloat interpolate( TonicFloat frame, unsigned int channel = 0 ) const;

    //! Returns the total number of audio samples represented by the object.
    size_t size() const { return size_; }; 

    //! Returns \e true if the object size is zero and \e false otherwise.
    bool empty() const;

    //! Resize self to represent the specified number of channels and frames.
    /*!
      Changes the size of self based on the number of frames and
      channels.  No element assignment is performed.  No memory
      deallocation occurs if the new size is smaller than the previous
      size.  Further, no new memory is allocated when the new size is
      smaller or equal to a previously allocated size.
    */
    void resize( size_t nFrames, unsigned int nChannels = 1 );

    //! Resize self to represent the specified number of channels and frames and perform element initialization.
    /*!
      Changes the size of self based on the number of frames and
      channels, and assigns \c value to every element.  No memory
      deallocation occurs if the new size is smaller than the previous
      size.  Further, no new memory is allocated when the new size is
      smaller or equal to a previously allocated size.
    */
    void resize( size_t nFrames, unsigned int nChannels, TonicFloat value );

    //! Return the number of channels represented by the data.
    unsigned int channels( void ) const { return nChannels_; };

    //! Return the number of sample frames represented by the data.
    unsigned int frames( void ) const { return nFrames_; };

    //! Set the sample rate associated with the TonicFrames data.
    /*!
      By default, this value is set equal to the current STK sample
      rate at the time of instantiation.
     */
    void setDataRate( TonicFloat rate ) { dataRate_ = rate; };

    //! Return the sample rate associated with the TonicFrames data.
    /*!
      By default, this value is set equal to the current STK sample
      rate at the time of instantiation.
     */
    TonicFloat dataRate( void ) const { return dataRate_; };

  private:

    TonicFloat *data_;
    TonicFloat dataRate_;
    size_t nFrames_;
    unsigned int nChannels_;
    size_t size_;
    size_t bufferSize_;

  };

  inline bool TonicFrames :: empty() const
  {
    if ( size_ > 0 ) return false;
    else return true;
  }

  inline TonicFloat& TonicFrames :: operator[] ( size_t n )
  {
  #if defined(_STK_DEBUG_)
    if ( n >= size_ ) {
      std::ostringstream error;
      error << "TonicFrames::operator[]: invalid index (" << n << ") value!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
  #endif

    return data_[n];
  }

  inline TonicFloat TonicFrames :: operator[] ( size_t n ) const
  {
  #if defined(_STK_DEBUG_)
    if ( n >= size_ ) {
      std::ostringstream error;
      error << "TonicFrames::operator[]: invalid index (" << n << ") value!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
  #endif

    return data_[n];
  }

  inline TonicFloat& TonicFrames :: operator() ( size_t frame, unsigned int channel )
  {
  #if defined(_STK_DEBUG_)
    if ( frame >= nFrames_ || channel >= nChannels_ ) {
      std::ostringstream error;
      error << "TonicFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
  #endif

    return data_[ frame * nChannels_ + channel ];
  }

  inline TonicFloat TonicFrames :: operator() ( size_t frame, unsigned int channel ) const
  {
  #if defined(_STK_DEBUG_)
    if ( frame >= nFrames_ || channel >= nChannels_ ) {
      std::ostringstream error;
      error << "TonicFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
  #endif

    return data_[ frame * nChannels_ + channel ];
  }

  inline void TonicFrames :: operator+= ( TonicFrames& f )
  {
  #if defined(_STK_DEBUG_)
    if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
      std::ostringstream error;
      error << "TonicFrames::operator+=: frames argument must be of equal dimensions!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
  #endif

    TonicFloat *fptr = &f[0];
    TonicFloat *dptr = data_;
    for ( unsigned int i=0; i<size_; i++ )
      *dptr++ += *fptr++;
  }

  inline void TonicFrames :: operator*= ( TonicFrames& f )
  {
  #if defined(_STK_DEBUG_)
    if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
      std::ostringstream error;
      error << "TonicFrames::operator*=: frames argument must be of equal dimensions!";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
  #endif

    TonicFloat *fptr = &f[0];
    TonicFloat *dptr = data_;
    for ( unsigned int i=0; i<size_; i++ )
      *dptr++ *= *fptr++;
  }

  
}

#endif /* defined(___013_1_23_melody__TonicFrames__) */