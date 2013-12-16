%module body

%include "std_string.i"
%include "std_shared_ptr.i"
%shared_ptr(ServoMover)
%shared_ptr(ArmGeometry)
%shared_ptr(LeftArmGeometry)
%shared_ptr(RightArmGeometry)
%{
#define SWIG_FILE_WITH_INIT
#include "ArmGeometry.h"
#include "RightArmGeometry.h"
#include "LeftArmGeometry.h"
#include "LegGeometry.h"

#include "Neck.h"
#include "Arms.h"
#include "Leg.h"
#include "Legs.h"
#include "Body.h"

#include "ArmMover.h"
#include "LegMover.h"
#include "Mover.h"
#include "RightArmMover.h"
#include "BodyMover.h"
#include "LeftArmMover.h"
#include "LegsMover.h"
#include "NeckMover.h"
#include "ServoMover.h"

#include "BodyGlobals.h"
#include "Tape.h"
%}
%include "ArmGeometry.h"
%include "RightArmGeometry.h"
%include "LeftArmGeometry.h"
%include "LegGeometry.h"

%include "Neck.h"
%include "Arms.h"
%include "Leg.h"
%include "Legs.h"
%include "Body.h"

%include "ArmMover.h"
%include "LegMover.h"
%include "Mover.h"
%include "RightArmMover.h"
%include "BodyMover.h"
%include "LeftArmMover.h"
%include "LegsMover.h"
%include "NeckMover.h"
%include "ServoMover.h"

%include "BodyGlobals.h"
%include "Tape.h"

