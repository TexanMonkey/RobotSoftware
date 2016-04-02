#
# Generic System Interface (GSI)
#

The classes defined in this library are generic interfaces to 
system level resources or to resources that are interfaced
differently on different system.  

Most of the classes in this library have direct dependancies
on the operating system, but provide a generic interface so
other classes will not need platform specific logic.

The public interface to all classes in this library MUST be
the same on all platforms; however, the implementation will
likely be different.  Therefore the classes will contain the
following preprocessor if structure in which additional 
Operating systems will be added as needed


#if defined(PTHREADS)

#elif defined(VXWORKS) || defined(_WRS_KERNEL)

#elif defined(_WINDOWS)

#else
#pragma warning("Platform is not supported")

#endif
