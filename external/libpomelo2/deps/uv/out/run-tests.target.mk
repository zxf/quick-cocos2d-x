# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := run-tests
DEFS_Debug := \
	'-DWIN32' \
	'-D_CRT_SECURE_NO_DEPRECATE' \
	'-D_CRT_NONSTDC_NO_DEPRECATE' \
	'-DDEBUG' \
	'-D_DEBUG'

# Flags passed to all source files.
CFLAGS_Debug := \
	-g \
	-O0 \
	-fwrapv

# Flags passed to only C files.
CFLAGS_C_Debug :=

# Flags passed to only C++ files.
CFLAGS_CC_Debug :=

INCS_Debug := \
	-I$(srcdir)/include

DEFS_Release := \
	'-DWIN32' \
	'-D_CRT_SECURE_NO_DEPRECATE' \
	'-D_CRT_NONSTDC_NO_DEPRECATE' \
	'-DNDEBUG'

# Flags passed to all source files.
CFLAGS_Release := \
	-O3 \
	-fstrict-aliasing \
	-fomit-frame-pointer \
	-fdata-sections \
	-ffunction-sections

# Flags passed to only C files.
CFLAGS_C_Release :=

# Flags passed to only C++ files.
CFLAGS_CC_Release :=

INCS_Release := \
	-I$(srcdir)/include

OBJS := \
	$(obj).target/$(TARGET)/test/blackhole-server.o \
	$(obj).target/$(TARGET)/test/echo-server.o \
	$(obj).target/$(TARGET)/test/run-tests.o \
	$(obj).target/$(TARGET)/test/runner.o \
	$(obj).target/$(TARGET)/test/test-get-loadavg.o \
	$(obj).target/$(TARGET)/test/test-active.o \
	$(obj).target/$(TARGET)/test/test-async.o \
	$(obj).target/$(TARGET)/test/test-async-null-cb.o \
	$(obj).target/$(TARGET)/test/test-callback-stack.o \
	$(obj).target/$(TARGET)/test/test-callback-order.o \
	$(obj).target/$(TARGET)/test/test-close-fd.o \
	$(obj).target/$(TARGET)/test/test-close-order.o \
	$(obj).target/$(TARGET)/test/test-connection-fail.o \
	$(obj).target/$(TARGET)/test/test-cwd-and-chdir.o \
	$(obj).target/$(TARGET)/test/test-default-loop-close.o \
	$(obj).target/$(TARGET)/test/test-delayed-accept.o \
	$(obj).target/$(TARGET)/test/test-error.o \
	$(obj).target/$(TARGET)/test/test-embed.o \
	$(obj).target/$(TARGET)/test/test-emfile.o \
	$(obj).target/$(TARGET)/test/test-fail-always.o \
	$(obj).target/$(TARGET)/test/test-fs.o \
	$(obj).target/$(TARGET)/test/test-fs-event.o \
	$(obj).target/$(TARGET)/test/test-get-currentexe.o \
	$(obj).target/$(TARGET)/test/test-get-memory.o \
	$(obj).target/$(TARGET)/test/test-getaddrinfo.o \
	$(obj).target/$(TARGET)/test/test-getnameinfo.o \
	$(obj).target/$(TARGET)/test/test-getsockname.o \
	$(obj).target/$(TARGET)/test/test-handle-fileno.o \
	$(obj).target/$(TARGET)/test/test-hrtime.o \
	$(obj).target/$(TARGET)/test/test-idle.o \
	$(obj).target/$(TARGET)/test/test-ip6-addr.o \
	$(obj).target/$(TARGET)/test/test-ipc.o \
	$(obj).target/$(TARGET)/test/test-ipc-send-recv.o \
	$(obj).target/$(TARGET)/test/test-loop-handles.o \
	$(obj).target/$(TARGET)/test/test-loop-alive.o \
	$(obj).target/$(TARGET)/test/test-loop-close.o \
	$(obj).target/$(TARGET)/test/test-loop-stop.o \
	$(obj).target/$(TARGET)/test/test-loop-time.o \
	$(obj).target/$(TARGET)/test/test-loop-configure.o \
	$(obj).target/$(TARGET)/test/test-walk-handles.o \
	$(obj).target/$(TARGET)/test/test-watcher-cross-stop.o \
	$(obj).target/$(TARGET)/test/test-multiple-listen.o \
	$(obj).target/$(TARGET)/test/test-osx-select.o \
	$(obj).target/$(TARGET)/test/test-pass-always.o \
	$(obj).target/$(TARGET)/test/test-ping-pong.o \
	$(obj).target/$(TARGET)/test/test-pipe-bind-error.o \
	$(obj).target/$(TARGET)/test/test-pipe-connect-error.o \
	$(obj).target/$(TARGET)/test/test-pipe-getsockname.o \
	$(obj).target/$(TARGET)/test/test-pipe-sendmsg.o \
	$(obj).target/$(TARGET)/test/test-pipe-server-close.o \
	$(obj).target/$(TARGET)/test/test-pipe-close-stdout-read-stdin.o \
	$(obj).target/$(TARGET)/test/test-pipe-set-non-blocking.o \
	$(obj).target/$(TARGET)/test/test-platform-output.o \
	$(obj).target/$(TARGET)/test/test-poll.o \
	$(obj).target/$(TARGET)/test/test-poll-close.o \
	$(obj).target/$(TARGET)/test/test-poll-close-doesnt-corrupt-stack.o \
	$(obj).target/$(TARGET)/test/test-poll-closesocket.o \
	$(obj).target/$(TARGET)/test/test-process-title.o \
	$(obj).target/$(TARGET)/test/test-ref.o \
	$(obj).target/$(TARGET)/test/test-run-nowait.o \
	$(obj).target/$(TARGET)/test/test-run-once.o \
	$(obj).target/$(TARGET)/test/test-semaphore.o \
	$(obj).target/$(TARGET)/test/test-shutdown-close.o \
	$(obj).target/$(TARGET)/test/test-shutdown-eof.o \
	$(obj).target/$(TARGET)/test/test-shutdown-twice.o \
	$(obj).target/$(TARGET)/test/test-signal.o \
	$(obj).target/$(TARGET)/test/test-signal-multiple-loops.o \
	$(obj).target/$(TARGET)/test/test-socket-buffer-size.o \
	$(obj).target/$(TARGET)/test/test-spawn.o \
	$(obj).target/$(TARGET)/test/test-fs-poll.o \
	$(obj).target/$(TARGET)/test/test-stdio-over-pipes.o \
	$(obj).target/$(TARGET)/test/test-tcp-bind-error.o \
	$(obj).target/$(TARGET)/test/test-tcp-bind6-error.o \
	$(obj).target/$(TARGET)/test/test-tcp-close.o \
	$(obj).target/$(TARGET)/test/test-tcp-close-accept.o \
	$(obj).target/$(TARGET)/test/test-tcp-close-while-connecting.o \
	$(obj).target/$(TARGET)/test/test-tcp-connect-error-after-write.o \
	$(obj).target/$(TARGET)/test/test-tcp-shutdown-after-write.o \
	$(obj).target/$(TARGET)/test/test-tcp-flags.o \
	$(obj).target/$(TARGET)/test/test-tcp-connect-error.o \
	$(obj).target/$(TARGET)/test/test-tcp-connect-timeout.o \
	$(obj).target/$(TARGET)/test/test-tcp-connect6-error.o \
	$(obj).target/$(TARGET)/test/test-tcp-open.o \
	$(obj).target/$(TARGET)/test/test-tcp-write-to-half-open-connection.o \
	$(obj).target/$(TARGET)/test/test-tcp-write-after-connect.o \
	$(obj).target/$(TARGET)/test/test-tcp-writealot.o \
	$(obj).target/$(TARGET)/test/test-tcp-try-write.o \
	$(obj).target/$(TARGET)/test/test-tcp-unexpected-read.o \
	$(obj).target/$(TARGET)/test/test-tcp-oob.o \
	$(obj).target/$(TARGET)/test/test-tcp-read-stop.o \
	$(obj).target/$(TARGET)/test/test-tcp-write-queue-order.o \
	$(obj).target/$(TARGET)/test/test-threadpool.o \
	$(obj).target/$(TARGET)/test/test-threadpool-cancel.o \
	$(obj).target/$(TARGET)/test/test-thread-equal.o \
	$(obj).target/$(TARGET)/test/test-mutexes.o \
	$(obj).target/$(TARGET)/test/test-thread.o \
	$(obj).target/$(TARGET)/test/test-barrier.o \
	$(obj).target/$(TARGET)/test/test-condvar.o \
	$(obj).target/$(TARGET)/test/test-timer-again.o \
	$(obj).target/$(TARGET)/test/test-timer-from-check.o \
	$(obj).target/$(TARGET)/test/test-timer.o \
	$(obj).target/$(TARGET)/test/test-tty.o \
	$(obj).target/$(TARGET)/test/test-udp-bind.o \
	$(obj).target/$(TARGET)/test/test-udp-dgram-too-big.o \
	$(obj).target/$(TARGET)/test/test-udp-ipv6.o \
	$(obj).target/$(TARGET)/test/test-udp-open.o \
	$(obj).target/$(TARGET)/test/test-udp-options.o \
	$(obj).target/$(TARGET)/test/test-udp-send-and-recv.o \
	$(obj).target/$(TARGET)/test/test-udp-send-immediate.o \
	$(obj).target/$(TARGET)/test/test-udp-send-unreachable.o \
	$(obj).target/$(TARGET)/test/test-udp-multicast-join.o \
	$(obj).target/$(TARGET)/test/test-udp-multicast-join6.o \
	$(obj).target/$(TARGET)/test/test-dlerror.o \
	$(obj).target/$(TARGET)/test/test-udp-multicast-ttl.o \
	$(obj).target/$(TARGET)/test/test-ip4-addr.o \
	$(obj).target/$(TARGET)/test/test-udp-multicast-interface.o \
	$(obj).target/$(TARGET)/test/test-udp-multicast-interface6.o \
	$(obj).target/$(TARGET)/test/test-udp-try-send.o \
	$(obj).target/$(TARGET)/test/runner-win.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# Make sure our dependencies are built before any of us.
$(OBJS): | $(obj).target/libuv.a

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.c FORCE_DO_CMD
	@$(call do_cmd,cc,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Debug :=

LDFLAGS_Release :=

LIBS := \
	-lws2_32 \
	-ladvapi32 \
	-liphlpapi \
	-lpsapi \
	-lshell32

$(builddir)/run-tests: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(builddir)/run-tests: LIBS := $(LIBS)
$(builddir)/run-tests: LD_INPUTS := $(OBJS) $(obj).target/libuv.a
$(builddir)/run-tests: TOOLSET := $(TOOLSET)
$(builddir)/run-tests: $(OBJS) $(obj).target/libuv.a FORCE_DO_CMD
	$(call do_cmd,link)

all_deps += $(builddir)/run-tests
# Add target alias
.PHONY: run-tests
run-tests: $(builddir)/run-tests

# Add executable to "all" target.
.PHONY: all
all: $(builddir)/run-tests

