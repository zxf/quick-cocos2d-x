# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := libuv
DEFS_Debug := \
	'-DWIN32' \
	'-D_CRT_SECURE_NO_DEPRECATE' \
	'-D_CRT_NONSTDC_NO_DEPRECATE' \
	'-D_WIN32_WINNT=0x0600' \
	'-D_GNU_SOURCE' \
	'-DDEBUG' \
	'-D_DEBUG'

# Flags passed to all source files.
CFLAGS_Debug := \
	-Wstrict-aliasing \
	-g \
	-O0 \
	-fwrapv

# Flags passed to only C files.
CFLAGS_C_Debug :=

# Flags passed to only C++ files.
CFLAGS_CC_Debug :=

INCS_Debug := \
	-I$(srcdir)/include \
	-I$(srcdir)/src

DEFS_Release := \
	'-DWIN32' \
	'-D_CRT_SECURE_NO_DEPRECATE' \
	'-D_CRT_NONSTDC_NO_DEPRECATE' \
	'-D_WIN32_WINNT=0x0600' \
	'-D_GNU_SOURCE' \
	'-DNDEBUG'

# Flags passed to all source files.
CFLAGS_Release := \
	-Wstrict-aliasing \
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
	-I$(srcdir)/include \
	-I$(srcdir)/src

OBJS := \
	$(obj).target/$(TARGET)/src/fs-poll.o \
	$(obj).target/$(TARGET)/src/inet.o \
	$(obj).target/$(TARGET)/src/threadpool.o \
	$(obj).target/$(TARGET)/src/uv-common.o \
	$(obj).target/$(TARGET)/src/version.o \
	$(obj).target/$(TARGET)/src/win/async.o \
	$(obj).target/$(TARGET)/src/win/core.o \
	$(obj).target/$(TARGET)/src/win/dl.o \
	$(obj).target/$(TARGET)/src/win/error.o \
	$(obj).target/$(TARGET)/src/win/fs.o \
	$(obj).target/$(TARGET)/src/win/fs-event.o \
	$(obj).target/$(TARGET)/src/win/getaddrinfo.o \
	$(obj).target/$(TARGET)/src/win/getnameinfo.o \
	$(obj).target/$(TARGET)/src/win/handle.o \
	$(obj).target/$(TARGET)/src/win/loop-watcher.o \
	$(obj).target/$(TARGET)/src/win/pipe.o \
	$(obj).target/$(TARGET)/src/win/thread.o \
	$(obj).target/$(TARGET)/src/win/poll.o \
	$(obj).target/$(TARGET)/src/win/process.o \
	$(obj).target/$(TARGET)/src/win/process-stdio.o \
	$(obj).target/$(TARGET)/src/win/req.o \
	$(obj).target/$(TARGET)/src/win/signal.o \
	$(obj).target/$(TARGET)/src/win/stream.o \
	$(obj).target/$(TARGET)/src/win/tcp.o \
	$(obj).target/$(TARGET)/src/win/tty.o \
	$(obj).target/$(TARGET)/src/win/timer.o \
	$(obj).target/$(TARGET)/src/win/udp.o \
	$(obj).target/$(TARGET)/src/win/util.o \
	$(obj).target/$(TARGET)/src/win/winapi.o \
	$(obj).target/$(TARGET)/src/win/winsock.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

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

LIBS :=

$(obj).target/libuv.a: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(obj).target/libuv.a: LIBS := $(LIBS)
$(obj).target/libuv.a: TOOLSET := $(TOOLSET)
$(obj).target/libuv.a: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,alink)

all_deps += $(obj).target/libuv.a
# Add target alias
.PHONY: libuv
libuv: $(obj).target/libuv.a

# Add target alias to "all" target.
.PHONY: all
all: libuv
