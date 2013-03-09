//
//  Filters.cpp
//  Tonic
//
//  Created by Nick Donaldson on 2/22/13.
//  Copyright (c) 2013 Morgan Packard. All rights reserved.
//

#include "Filters.h"

namespace Tonic { namespace Tonic_{
  
  // ================================
  //       Filter Base Class
  // ================================
  
  Filter_::Filter_() :
    cutoff_(FixedValue(20000)),
    Q_(FixedValue(0.7071)),
    bypass_(ControlValue(0))
  {
    workspace_.resize(kSynthesisBlockSize, 1, 0);
  }
  
  // pretty sure this can be removed. It looks like a duplicate of what happens in Generator_::setIsStereo
  void Filter_::setIsStereo(bool stereo){
    Generator_::setIsStereo(stereo);
    workspace_.resize(kSynthesisBlockSize, stereo ? 2 : 1, 0);
  }
  
} // Namespace Tonic_
  
  
  
} // Namespace Tonic
