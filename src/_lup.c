#include <Python.h>
#include "lup.h"

/*
 *  Python API
 */
struct module_state {
	    PyObject *error;
};


static struct module_state _state;

static void del_LuaState(PyObject *self) {
   lua_State *L;
   if ((L = (lua_State*)PyCapsule_GetPointer(self, "LuaState"))) {
      lua_close(L);
   }
}

static PyObject *_lup_LuaState(PyObject *self, PyObject *args) {
   lua_State *L = luaL_newstate();
   luaL_openlibs(L);
   init(L);
   return PyCapsule_New(L, "LuaState", del_LuaState);
}

static PyObject *_lup_process_chunk(PyObject *self, PyObject *args) {
   char *input;
   char *output;
   PyObject *py_luaState;
   PyObject *py_output;
   lua_State *L;

   if (!PyArg_ParseTuple(args, "Os", &py_luaState, &input)) {
      return NULL;
   }
   if (!(L = (lua_State*)PyCapsule_GetPointer(py_luaState, "LuaState"))) {
      return NULL;
   }

   output = process_chunk(L, input);
   if (output) {
      py_output = PyUnicode_FromString(output);
      free(output);
   }
   else {
      py_output = Py_None;
      Py_INCREF(py_output);
   }

   return py_output;
}


static PyMethodDef lupMethods[] = {
   {"process_chunk", _lup_process_chunk, METH_VARARGS, "Process this Lua string"},
   {"LuaState", _lup_LuaState, METH_VARARGS, "Create a new Lua State"},
   {NULL, NULL, 0, NULL}
};


static struct PyModuleDef moduledef = {
       PyModuleDef_HEAD_INIT,
       "lup",
        NULL,
        sizeof(struct module_state),
        lupMethods,
        NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC
PyInit_lup(void)
{
//#   Py_InitModule("lup", lupMethods);
PyObject *module = PyModule_Create(&moduledef);
 return module;
}
