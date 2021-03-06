# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.4
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.



from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_zmqservos', [dirname(__file__)])
        except ImportError:
            import _zmqservos
            return _zmqservos
        if fp is not None:
            try:
                _mod = imp.load_module('_zmqservos', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _zmqservos = swig_import_helper()
    del swig_import_helper
else:
    import _zmqservos
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


SHARED_PTR_DISOWN = _zmqservos.SHARED_PTR_DISOWN
USE_SERVO_LINEAR = _zmqservos.USE_SERVO_LINEAR
class Servo(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Servo, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Servo, name)
    def __init__(self, *args, **kwargs): raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    def curve(self, *args): return _zmqservos.Servo_curve(self, *args)
    def angle(self, *args): return _zmqservos.Servo_angle(self, *args)
    def speed(self, *args): return _zmqservos.Servo_speed(self, *args)
    def torque(self, *args): return _zmqservos.Servo_torque(self, *args)
    def rate(self, *args): return _zmqservos.Servo_rate(self, *args)
    __swig_destroy__ = _zmqservos.delete_Servo
    __del__ = lambda self : None;
    def name(self, *args): return _zmqservos.Servo_name(self, *args)
    def id(self, *args): return _zmqservos.Servo_id(self, *args)
    def report(self, *args): return _zmqservos.Servo_report(self, *args)
    def temp_report(self, *args): return _zmqservos.Servo_temp_report(self, *args)
    def temp(self): return _zmqservos.Servo_temp(self)
Servo_swigregister = _zmqservos.Servo_swigregister
Servo_swigregister(Servo)

class ZMQServo(Servo):
    __swig_setmethods__ = {}
    for _s in [Servo]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, ZMQServo, name, value)
    __swig_getmethods__ = {}
    for _s in [Servo]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, ZMQServo, name)
    __repr__ = _swig_repr
    __swig_setmethods__["goalAngle"] = _zmqservos.ZMQServo_goalAngle_set
    __swig_getmethods__["goalAngle"] = _zmqservos.ZMQServo_goalAngle_get
    if _newclass:goalAngle = _swig_property(_zmqservos.ZMQServo_goalAngle_get, _zmqservos.ZMQServo_goalAngle_set)
    __swig_setmethods__["presentAngle"] = _zmqservos.ZMQServo_presentAngle_set
    __swig_getmethods__["presentAngle"] = _zmqservos.ZMQServo_presentAngle_get
    if _newclass:presentAngle = _swig_property(_zmqservos.ZMQServo_presentAngle_get, _zmqservos.ZMQServo_presentAngle_set)
    __swig_setmethods__["goalSpeed"] = _zmqservos.ZMQServo_goalSpeed_set
    __swig_getmethods__["goalSpeed"] = _zmqservos.ZMQServo_goalSpeed_get
    if _newclass:goalSpeed = _swig_property(_zmqservos.ZMQServo_goalSpeed_get, _zmqservos.ZMQServo_goalSpeed_set)
    __swig_setmethods__["goalTorque"] = _zmqservos.ZMQServo_goalTorque_set
    __swig_getmethods__["goalTorque"] = _zmqservos.ZMQServo_goalTorque_get
    if _newclass:goalTorque = _swig_property(_zmqservos.ZMQServo_goalTorque_get, _zmqservos.ZMQServo_goalTorque_set)
    __swig_setmethods__["goalRate"] = _zmqservos.ZMQServo_goalRate_set
    __swig_getmethods__["goalRate"] = _zmqservos.ZMQServo_goalRate_get
    if _newclass:goalRate = _swig_property(_zmqservos.ZMQServo_goalRate_get, _zmqservos.ZMQServo_goalRate_set)
    __swig_setmethods__["curveMode"] = _zmqservos.ZMQServo_curveMode_set
    __swig_getmethods__["curveMode"] = _zmqservos.ZMQServo_curveMode_get
    if _newclass:curveMode = _swig_property(_zmqservos.ZMQServo_curveMode_get, _zmqservos.ZMQServo_curveMode_set)
    __swig_setmethods__["t"] = _zmqservos.ZMQServo_t_set
    __swig_getmethods__["t"] = _zmqservos.ZMQServo_t_get
    if _newclass:t = _swig_property(_zmqservos.ZMQServo_t_get, _zmqservos.ZMQServo_t_set)
    __swig_setmethods__["c0"] = _zmqservos.ZMQServo_c0_set
    __swig_getmethods__["c0"] = _zmqservos.ZMQServo_c0_get
    if _newclass:c0 = _swig_property(_zmqservos.ZMQServo_c0_get, _zmqservos.ZMQServo_c0_set)
    __swig_setmethods__["c1"] = _zmqservos.ZMQServo_c1_set
    __swig_getmethods__["c1"] = _zmqservos.ZMQServo_c1_get
    if _newclass:c1 = _swig_property(_zmqservos.ZMQServo_c1_get, _zmqservos.ZMQServo_c1_set)
    def curve(self, *args): return _zmqservos.ZMQServo_curve(self, *args)
    def __init__(self): 
        this = _zmqservos.new_ZMQServo()
        try: self.this.append(this)
        except: self.this = this
    def angle(self, *args): return _zmqservos.ZMQServo_angle(self, *args)
    def speed(self, *args): return _zmqservos.ZMQServo_speed(self, *args)
    def torque(self, *args): return _zmqservos.ZMQServo_torque(self, *args)
    def rate(self, *args): return _zmqservos.ZMQServo_rate(self, *args)
    def temp(self, *args): return _zmqservos.ZMQServo_temp(self, *args)
    __swig_destroy__ = _zmqservos.delete_ZMQServo
    __del__ = lambda self : None;
ZMQServo_swigregister = _zmqservos.ZMQServo_swigregister
ZMQServo_swigregister(ZMQServo)

class ZMQServoMessage(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ZMQServoMessage, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ZMQServoMessage, name)
    __repr__ = _swig_repr
    SET_ANGLE = _zmqservos.ZMQServoMessage_SET_ANGLE
    GET_ANGLE = _zmqservos.ZMQServoMessage_GET_ANGLE
    SET_SPEED = _zmqservos.ZMQServoMessage_SET_SPEED
    SET_TORQUE = _zmqservos.ZMQServoMessage_SET_TORQUE
    SET_CURVE = _zmqservos.ZMQServoMessage_SET_CURVE
    SET_RATE = _zmqservos.ZMQServoMessage_SET_RATE
    GET_TEMP = _zmqservos.ZMQServoMessage_GET_TEMP
    DO_NOTHING = _zmqservos.ZMQServoMessage_DO_NOTHING
    __swig_setmethods__["messageId"] = _zmqservos.ZMQServoMessage_messageId_set
    __swig_getmethods__["messageId"] = _zmqservos.ZMQServoMessage_messageId_get
    if _newclass:messageId = _swig_property(_zmqservos.ZMQServoMessage_messageId_get, _zmqservos.ZMQServoMessage_messageId_set)
    __swig_setmethods__["servoId"] = _zmqservos.ZMQServoMessage_servoId_set
    __swig_getmethods__["servoId"] = _zmqservos.ZMQServoMessage_servoId_get
    if _newclass:servoId = _swig_property(_zmqservos.ZMQServoMessage_servoId_get, _zmqservos.ZMQServoMessage_servoId_set)
    __swig_setmethods__["value"] = _zmqservos.ZMQServoMessage_value_set
    __swig_getmethods__["value"] = _zmqservos.ZMQServoMessage_value_get
    if _newclass:value = _swig_property(_zmqservos.ZMQServoMessage_value_get, _zmqservos.ZMQServoMessage_value_set)
    def __init__(self): 
        this = _zmqservos.new_ZMQServoMessage()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _zmqservos.delete_ZMQServoMessage
    __del__ = lambda self : None;
ZMQServoMessage_swigregister = _zmqservos.ZMQServoMessage_swigregister
ZMQServoMessage_swigregister(ZMQServoMessage)

class ZMQServoCurveMessage(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ZMQServoCurveMessage, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ZMQServoCurveMessage, name)
    __repr__ = _swig_repr
    __swig_setmethods__["messageId"] = _zmqservos.ZMQServoCurveMessage_messageId_set
    __swig_getmethods__["messageId"] = _zmqservos.ZMQServoCurveMessage_messageId_get
    if _newclass:messageId = _swig_property(_zmqservos.ZMQServoCurveMessage_messageId_get, _zmqservos.ZMQServoCurveMessage_messageId_set)
    __swig_setmethods__["servoId"] = _zmqservos.ZMQServoCurveMessage_servoId_set
    __swig_getmethods__["servoId"] = _zmqservos.ZMQServoCurveMessage_servoId_get
    if _newclass:servoId = _swig_property(_zmqservos.ZMQServoCurveMessage_servoId_get, _zmqservos.ZMQServoCurveMessage_servoId_set)
    __swig_setmethods__["t"] = _zmqservos.ZMQServoCurveMessage_t_set
    __swig_getmethods__["t"] = _zmqservos.ZMQServoCurveMessage_t_get
    if _newclass:t = _swig_property(_zmqservos.ZMQServoCurveMessage_t_get, _zmqservos.ZMQServoCurveMessage_t_set)
    __swig_setmethods__["c0"] = _zmqservos.ZMQServoCurveMessage_c0_set
    __swig_getmethods__["c0"] = _zmqservos.ZMQServoCurveMessage_c0_get
    if _newclass:c0 = _swig_property(_zmqservos.ZMQServoCurveMessage_c0_get, _zmqservos.ZMQServoCurveMessage_c0_set)
    __swig_setmethods__["c1"] = _zmqservos.ZMQServoCurveMessage_c1_set
    __swig_getmethods__["c1"] = _zmqservos.ZMQServoCurveMessage_c1_get
    if _newclass:c1 = _swig_property(_zmqservos.ZMQServoCurveMessage_c1_get, _zmqservos.ZMQServoCurveMessage_c1_set)
    def __init__(self): 
        this = _zmqservos.new_ZMQServoCurveMessage()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _zmqservos.delete_ZMQServoCurveMessage
    __del__ = lambda self : None;
ZMQServoCurveMessage_swigregister = _zmqservos.ZMQServoCurveMessage_swigregister
ZMQServoCurveMessage_swigregister(ZMQServoCurveMessage)

class ServoController(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ServoController, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ServoController, name)
    def __init__(self, *args, **kwargs): raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    def servo(self, *args): return _zmqservos.ServoController_servo(self, *args)
    def start(self): return _zmqservos.ServoController_start(self)
    __swig_destroy__ = _zmqservos.delete_ServoController
    __del__ = lambda self : None;
ServoController_swigregister = _zmqservos.ServoController_swigregister
ServoController_swigregister(ServoController)


def CreateFakeServoController():
  return _zmqservos.CreateFakeServoController()
CreateFakeServoController = _zmqservos.CreateFakeServoController

def CreateZMQServoController(*args):
  return _zmqservos.CreateZMQServoController(*args)
CreateZMQServoController = _zmqservos.CreateZMQServoController

def CreateZMQServoListener(*args):
  return _zmqservos.CreateZMQServoListener(*args)
CreateZMQServoListener = _zmqservos.CreateZMQServoListener
# This file is compatible with both classic and new-style classes.


