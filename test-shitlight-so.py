import ctypes
from ctypes.util import find_library

# define frame class
class t_chitframe (ctypes.Structure):
    _fields_ = [("brightness", ((ctypes.c_uint16 * 3) * 8 * 2))]


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

_chit.add_frame.argtypes = [ctypes.c_uint16, ctypes.POINTER(t_chitframe)]
_chit.add_frame.restype = None

_chit.get_fps.argtypes = None
_chit.get_fps.restype = ctypes.c_float

_chit.main.argtypes = None
_chit.main.restype = ctypes.c_int

_chit.main()
