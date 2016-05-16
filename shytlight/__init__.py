import ctypes
from ctypes.util import find_library

# define frame class
class t_chitframe (ctypes.Structure):
    _fields_ = [("brightness", ((ctypes.c_uint16 * 3) * 8 * 2))]


# find the shitlight library
_chit = ctypes.cdll.LoadLibrary(find_library("libshitlight.so."))

# define the function types
# why doesn't ctypes do this for us?
_chit.init.argtypes = None
_chit.init.restype = ctypes.c_int

_chit.add_frame.argtypes = [ctypes.c_uint16, ctypes.POINTER(t_chitframe)]
_chit.add_frame.restype = None

_chit.get_fps.argtypes = None
_chit.get_fps.restype = ctypes.c_float

def init_shitlight():
    return (_chit.init()==1)

def get_fps():
    return _chit.get_fps()

def add_frame(rep, frame):
    # test if frame is internal format or needs to be converted
    if typeof(frame) is t_chitframe:
        _chit.add_frame(rep, ctypes.byref(frame))
    else:
        # not implemented yet
        raise NotImplementedError

