import ctypes
from ctypes.util import find_library
import numpy 
# define frame class
class t_chitframe (ctypes.Structure):
    _fields_ = [("brightness", ((ctypes.c_uint8 * 3) * 8 * 5))]


# find the shitlight library
_adr = find_library("libshitlight.so")
if _adr is None:
    # test environment? Try again in parent directory
    _chit = ctypes.cdll.LoadLibrary("./libshitlight.so")
    if _chit._name is None:
        # give up
        raise NameError
else:
    _chit = ctypes.cdll.LoadLibrary(_adr)
print(_chit)

# define the function types
# why doesn't ctypes do this for us?
_chit.init.argtypes = None
_chit.init.restype = ctypes.c_int

_chit.add_frame.argtypes = [ctypes.c_uint16, ctypes.c_uint8, ctypes.POINTER(t_chitframe)]
_chit.add_frame.restype = None

_chit.get_fps.argtypes = None
_chit.get_fps.restype = ctypes.c_float

_chit.get_fps_limit.argtypes = None
_chit.get_fps_limit.restype = ctypes.c_int

_chit.set_bpm.argtypes = ctypes.c_float
_chit.set_bpm.restypes = ctypes.c_int

_chit.get_bpm.argtypes = None
_chit.get_bpm.restypes = ctypes.c_float

_chit.get_analysis_state.argtypes = None
_chit.get_analysis_state.restypes = ctypes.c_int

_chit.init_analysis.argtypes = None
_chit.init_analysis.restypes = ctypes.c_int

_chit.stop_analysis.argtypes = None
_chit.stop_analysis.restypes = ctypes.c_int

_chit.beat_sync.argtypes = ctypes.c_uint8
_chit.beat_sync.restypes = ctypes.c_int

_chit.reset.argtypes = None
_chit.reset.restype = ctypes.c_int

def init_shitlight():
    return (_chit.init()==1)

def get_fps():
    return _chit.get_fps()

def get_bpm():
    return _chit.get_bpm()

def set_bpm(bpm):
    return _chit.set_bpm(bpm) == 1

def get_analysis_state():
    return _chit.get_analysis_state() >= 1

def init_analysis():
    return _chit.init_analysis() == 1

def stop_analysis():
    return _chit.stop_analysis() == 1

def beat_sync(enabled):
    return _chit.beat_sync(enabled) == 1

def beats(count):
    # converts (fraction of) beats to needed repetitions of frames
    return int(count*_chit.get_fps_limit*60/120)

def clear_buffer():
    print "Deleting Buffer..."
    return _chit.reset()

def add_frame(rep, frame,on_beat=False):
    # test if frame is internal format or needs to be converted
    if type(frame) is t_chitframe:
        _chit.add_frame(rep, on_beat, ctypes.byref(frame))
    elif type(frame) is numpy.ndarray:
        temp_frame = transform(frame) 
        _chit.add_frame(rep, on_beat, ctypes.byref(temp_frame))
    else:
        # not implemented yet
        raise NotImplementedError

def transform(frame):
     temp_frame = t_chitframe()
     for i in range(5):
          for j in range(8):
              for k in range(3):
                  temp_frame.brightness[i][j][k] = ctypes.c_uint8(frame[i,j,k].astype("ubyte"))
     return temp_frame
