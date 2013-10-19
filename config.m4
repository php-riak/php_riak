dnl lines starting with "dnl" are comments

PHP_ARG_ENABLE(riak, whether to enable Riak extension, [  --enable-riak   Enable Riak extension])
PHP_ARG_ENABLE(riak-session, whether to enable riak sessions, [ --disable-riak-session Disable riak session support], yes, no)
if test "$PHP_RIAK" != "no"; then
  if test "$PHP_RIAK_SESSION" != "no"; then
    AC_DEFINE(PHP_RIAK_SESSION,1,[riak session])
  fi

  dnl this defines the extension
  PHP_NEW_EXTENSION(riak, php_riak.c \
    ht_utils.c \
    riak_session.c \
    riak/connection.c \
    riak/bucket.c \
    riak/object.c \
    riak/exceptions.c \
    riak/bucket_properties.c \
    riak/pool.c \
    riak/link.c \
    riak/pool_info.c \
    riak/streaming.c \
    riak/req_inputs.c \
    riak/req_outputs.c \
    riak/property/module_function.c \
    riak/property/commit_hook.c \
    riak/property/commit_hook_list.c \
    riak/search/search.c \
    riak/search/input/parameter_bag.c \
    riak/search/output/document_output.c \
    riak/search/output/output.c \
    riak/map_reduce/mapreduce.c \
    riak/map_reduce/mr_phase.c \
    riak/map_reduce/mr_functions.c \
    riak/map_reduce/mr_inputs.c \
    riak/map_reduce/mr_output.c \
    LibRiack/src/riack_sock.c \
    LibRiack/src/riack.c \
    LibRiack/src/riack_kv.c \
    LibRiack/src/riack_search.c \
    LibRiack/src/riack_msg.c \
    LibRiack/src/riack_mem.c \
    LibRiack/src/riack_helpers.c \
    LibRiack/src/protocol/riak_msg_codes.c \
    LibRiack/src/protocol/riak_search.pb-c.c \
    LibRiack/src/protocol/riak.pb-c.c \
    LibRiack/src/protocol/riak_kv.pb-c.c \
    LibRiack/src/google/protobuf-c/protobuf-c.c, $ext_shared)

  PHP_ADD_BUILD_DIR([$ext_builddir/LibRiack/src], 1)
  PHP_ADD_INCLUDE([$ext_builddir/LibRiack/src])
  PHP_ADD_INCLUDE([$ext_srcdir/LibRiack/src])

  PHP_ADD_BUILD_DIR([$ext_builddir/LibRiack/src/protocol], 1)
  PHP_ADD_INCLUDE([$ext_builddir/LibRiack/src/protocol])
  PHP_ADD_INCLUDE([$ext_srcdir/LibRiack/src/protocol])

  PHP_ADD_BUILD_DIR([$ext_builddir/LibRiack/src/google/protobuf-c], 1)
  PHP_ADD_INCLUDE([$ext_builddir/LibRiack/src/google/protobuf-c])
  PHP_ADD_INCLUDE([$ext_srcdir/LibRiack/src/google/protobuf-c])

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
