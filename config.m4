dnl lines starting with "dnl" are comments

PHP_ARG_ENABLE(riak, whether to enable Riak extension, [  --enable-riak   Enable Riak extension])

if test "$PHP_RIAK" != "no"; then

  if test -r $PHP_RIAK/google/protobuf-c/protobuf-c.h; then
    PB_DIR=$PHP_RIAK
  else
    AC_MSG_CHECKING(for protobuf-c in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/google/protobuf-c/protobuf-c.h; then
        PB_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi
  if test -z "$PB_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libprotobuf-c distribution -
    protobuf-c.h should be in <protobuf-c dir>/include/google/protobuf-c/)
  fi

  PHP_SUBST(RIAK_SHARED_LIBADD)
  PHP_ADD_LIBPATH(riack)
  PHP_ADD_LIBRARY(riack,, RIAK_SHARED_LIBADD)
  PHP_ADD_LIBRARY(protobuf-c, 1, RIAK_SHARED_LIBADD)

  PHP_ADD_LIBPATH($PB_DIR/lib, RIAK_SHARED_LIBADD)
  PHP_ADD_LIBPATH(riack, RIAK_SHARED_LIBADD)
  PHP_ADD_INCLUDE([riack/src])

  PHP_SUBST(RIAK_SHARED_LIBADD)

  dnl this defines the extension
  PHP_NEW_EXTENSION(riak, php_riak.c client.c bucket.c object.c exceptions.c bucket_properties.c, $ext_shared)

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
