dnl lines starting with "dnl" are comments

PHP_ARG_ENABLE(riak, whether to enable Riak extension, [  --enable-riak   Enable Riak extension])
PHP_ARG_ENABLE(riak-session, whether to enable riak sessions, [ --disable-riak-session Disable riak session support], yes, no)

dnl
dnl Checks for the configure options
dnl

if test -z "$PHP_RIAK_C_DIR"; then
  PHP_ARG_WITH(riak-c-dir, for the location of Riak C,
  [  --with-riak-c-dir[=DIR]     Set the path to riak-c install prefix.], no, no)
fi

AC_DEFUN(RIAK_C_LIB,[
  if test "$PHP_RIAK_C_DIR" != "no"; then
    LIB_RIAK_C_NAME=riak_c_client-0.1

    for i in $PHP_RIAK_C_DIR /usr/local /usr; do
      if test -f "$i/include/riak.h"; then
        PHP_RIAK_C_DIR=$i
        break;
      fi
    done

    PHP_CHECK_LIBRARY($LIB_RIAK_C_NAME, riak_ping,
      [
        PHP_ADD_INCLUDE($PHP_RIAK_C_DIR)
        PHP_ADD_INCLUDE($PHP_RIAK_C_DIR/include)
        PHP_ADD_LIBRARY_WITH_PATH($LIB_RIAK_C_NAME, $PHP_RIAK_C_DIR/lib, RIAK_SHARED_LIBADD)
      ],[
        AC_MSG_ERROR([Problem with $LIB_RIAK_C_NAME.(a|so). Please check config.log for more information.])
      ],[
        -L$PHP_RIAK_C_DIR/lib
      ])
  else
    AC_MSG_RESULT([If configure fails try --with-riak-c-dir=<DIR>])
  fi
])

if test "$PHP_RIAK_SESSION" != "no"; then
  AC_DEFINE(PHP_RIAK_SESSION,1,[riak session])
fi

dnl
dnl this defines the extension
dnl
if test "$PHP_RIAK" != "no"; then
  RIAK_C_LIB

  PHP_NEW_EXTENSION(riak, php_riak.c \
    ht_utils.c \
    riak_session.c \
    riak_c_helpers.c \
    riak/bucket.c \
    riak/connection.c \
    riak/object.c \
    riak/exception/exception.c \
    riak/bucket_properties.c \
    riak/pool.c \
    riak/link.c \
    riak/pool_info.c \
    riak/server_info.c \
    riak/object_list.c \
    riak/crdt/counter.c \
    riak/crdt/input/get_input.c \
    riak/crdt/input/update_input.c \
    riak/input/input.c \
    riak/input/delete_input.c \
    riak/input/get_input.c \
    riak/input/get_resolver_input.c \
    riak/input/put_input.c \
    riak/input/index_input.c \
    riak/query/index_query.c \
    riak/output/output.c \
    riak/output/get_output.c \
    riak/output/put_output.c \
    riak/output/conflict_resolver.c \
    riak/output/youngest_sibling_resolver.c \
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
    , $ext_shared)

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

  PHP_SUBST(RIAK_SHARED_LIBADD)

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
