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
    riak/bucket.c \
    riak/connection.c \
    riak/object.c \
    riak/exception/exception.c \
    riak/bucket_properties.c \
    riak/pool.c \
    riak/link.c \
    riak/pool_info.c \
    riak/object_list.c \
    riak/crdt/counter.c \
    riak/crdt/input/get_input.c \
    riak/crdt/input/update_input.c \
    riak/input/input.c \
    riak/input/delete_input.c \
    riak/input/get_input.c \
    riak/input/put_input.c \
    riak/input/index_input.c \
    riak/query/index_query.c \
    riak/output/output.c \
    riak/output/get_output.c \
    riak/output/put_output.c \
    riak/output/index_result.c \
    riak/output/index_result_list.c \
    riak/output/index_output.c \
    riak/output/key_stream_output.c \
    riak/property/module_function.c \
    riak/property/commit_hook.c \
    riak/property/commit_hook_list.c \
    riak/property/replication_mode/replication_mode.c \
    riak/search/search.c \
    riak/search/input/parameter_bag.c \
    riak/search/output/document_output.c \
    riak/search/output/output.c \
    riak/map_reduce/mapreduce.c \
    riak/map_reduce/phase/phase.c \
    riak/map_reduce/phase/map_phase.c \
    riak/map_reduce/phase/reduce_phase.c \
    riak/map_reduce/output/output.c \
    riak/map_reduce/output/stream_output.c \
    riak/map_reduce/input/input.c \
    riak/map_reduce/input/bucket_input.c \
    riak/map_reduce/input/key_data_list_input.c \
    riak/map_reduce/input/key_list_input.c \
    riak/map_reduce/functions/base_function.c \
    riak/map_reduce/functions/javascript_function.c \
    riak/map_reduce/functions/erlang_function.c \
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

    PHP_ADD_BUILD_DIR([$ext_builddir/riak], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/crdt], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/crdt/input], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/exception], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/input], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/output], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/property], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/property/replication_mode], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/map_reduce], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/map_reduce/functions], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/map_reduce/input], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/map_reduce/output], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/map_reduce/phase], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/search], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/query], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/search/input], 1)
    PHP_ADD_BUILD_DIR([$ext_builddir/riak/search/output], 1)

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
