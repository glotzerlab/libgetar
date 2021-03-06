/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package libgetar_wrap;

public class SharedArrayChar {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected SharedArrayChar(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(SharedArrayChar obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libgetar_wrapJNI.delete_SharedArrayChar(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public SharedArrayChar() {
    this(libgetar_wrapJNI.new_SharedArrayChar__SWIG_0(), true);
  }

  public SharedArrayChar(SWIGTYPE_p_char target, long length) {
    this(libgetar_wrapJNI.new_SharedArrayChar__SWIG_1(SWIGTYPE_p_char.getCPtr(target), length), true);
  }

  public SharedArrayChar(SharedArrayChar rhs) {
    this(libgetar_wrapJNI.new_SharedArrayChar__SWIG_2(SharedArrayChar.getCPtr(rhs), rhs), true);
  }

  public void copy(SharedArrayChar rhs) {
    libgetar_wrapJNI.SharedArrayChar_copy(swigCPtr, this, SharedArrayChar.getCPtr(rhs), rhs);
  }

  public boolean isNull() {
    return libgetar_wrapJNI.SharedArrayChar_isNull(swigCPtr, this);
  }

  public SWIGTYPE_p_char begin() {
    long cPtr = libgetar_wrapJNI.SharedArrayChar_begin(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_char(cPtr, false);
  }

  public SWIGTYPE_p_char end() {
    long cPtr = libgetar_wrapJNI.SharedArrayChar_end(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_char(cPtr, false);
  }

  public SWIGTYPE_p_char get() {
    long cPtr = libgetar_wrapJNI.SharedArrayChar_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_char(cPtr, false);
  }

  public long size() {
    return libgetar_wrapJNI.SharedArrayChar_size(swigCPtr, this);
  }

  public void release() {
    libgetar_wrapJNI.SharedArrayChar_release(swigCPtr, this);
  }

  public void swap(SharedArrayChar target) {
    libgetar_wrapJNI.SharedArrayChar_swap(swigCPtr, this, SharedArrayChar.getCPtr(target), target);
  }

}
