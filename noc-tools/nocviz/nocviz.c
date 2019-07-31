#include "nocviz.h"


	static int
Hello_Cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Tcl_SetObjResult(interp, Tcl_NewStringObj("Hello, World!", -1));
	return TCL_OK;
}

/*
 * Hello_Init -- Called when Tcl loads your extension.
 */
	int DLLEXPORT
Nocviz_Init(Tcl_Interp *interp)
{
	Tcl_Namespace *nsPtr; /* pointer to hold our own new namespace */

	if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
		return TCL_ERROR;
	}

	/* create the namespace named 'hello' */
	nsPtr = Tcl_CreateNamespace(interp, "hello", NULL, NULL);
	if (nsPtr == NULL) {
		return TCL_ERROR;
	}

	/* just prepend the namespace to the name of the command.
	   Tcl will now create the 'hello' command in the 'hello'
	   namespace so it can be called as 'hello::hello' */
	Tcl_CreateObjCommand(interp, "hello::hello", Hello_Cmd, NULL, NULL);
	Tcl_PkgProvide(interp, "Hello", "1.0");
	return TCL_OK;
}
