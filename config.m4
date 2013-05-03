dnl lines starting with "dnl" are comments

PHP_ARG_ENABLE(riak, whether to enable Riak extension, [  --enable-riak   Enable Riak extension])

if test "$PHP_RIAK" != "no"; then

  PHP_SUBST(RIAK_SHARED_LIBADD)
  PHP_ADD_LIBPATH(riack)
  PHP_ADD_LIBRARY(riack,, RIAK_SHARED_LIBADD)
  PHP_ADD_INCLUDE([riack/src])

  dnl this defines the extension
  PHP_NEW_EXTENSION(riak, php_riak.c client.c bucket.c object.c exceptions.c, $ext_shared)

  dnl this is boilerplate to make the extension work on OS X
  case $build_os in
  darwin1*.*.*)
    AC_MSG_CHECKING([whether to compile for recent osx architectures])
    CFLAGS="$CFLAGS -arch i386 -arch x86_64 -mmacosx-version-min=10.5"
    AC_MSG_RESULT([yes])
    ;;
  darwin*)
    AC_MSG_CHECKING([whether to compile for every osx architecture ever])
    CFLAGS="$CFLAGS -arch i386 -arch x86_64 -arch ppc -arch ppc64"
    AC_MSG_RESULT([yes])
    ;;
  esac
fi
