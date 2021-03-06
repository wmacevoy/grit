# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(TARGET_ARCH),)
  TARGET_ARCH := -march=native
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "LEAP_SCENE_USE_UTIL_GL=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../../../LeapSDK/include -I ../../../../LeapSDK/util
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS) 
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib -L"../../../../LeapSDK/lib/$(shell [[ $$(uname -m) == x86_64 && "$(TARGET_ARCH)" == "-march=native" ]] && echo x64 || echo x86)" -ldl -lfreetype -lpthread -lrt -lX11 -lGL -lGLU -lXinerama -lasound -lXext -lLeap -Wl,-rpath,\$$ORIGIN/../../../../../LeapSDK/lib/$(shell [[ $$(uname -m) == x86_64 && "$(TARGET_ARCH)" == "-march=native" ]] && echo x64 || echo x86),-rpath,\$$ORIGIN/../../LeapSDK/lib/$(shell [[ $$(uname -m) == x86_64 && "$(TARGET_ARCH)" == "-march=native" ]] && echo x64 || echo x86)
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "LEAP_SCENE_USE_UTIL_GL=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../../../LeapSDK/include -I ../../../../LeapSDK/util
  TARGET := MotionVisualizer
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "LEAP_SCENE_USE_UTIL_GL=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../../../LeapSDK/include -I ../../../../LeapSDK/util
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -Os
  CXXFLAGS += $(CFLAGS) 
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib -L"../../../../LeapSDK/lib/$(shell [[ $$(uname -m) == x86_64 && "$(TARGET_ARCH)" == "-march=native" ]] && echo x64 || echo x86)" -ldl -lfreetype -lpthread -lrt -lX11 -lGL -lGLU -lXinerama -lasound -lXext -lLeap -Wl,-rpath,\$$ORIGIN/../../../../../LeapSDK/lib/$(shell [[ $$(uname -m) == x86_64 && "$(TARGET_ARCH)" == "-march=native" ]] && echo x64 || echo x86),-rpath,\$$ORIGIN/../../LeapSDK/lib/$(shell [[ $$(uname -m) == x86_64 && "$(TARGET_ARCH)" == "-march=native" ]] && echo x64 || echo x86)
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "NDEBUG=1" -D "LEAP_SCENE_USE_UTIL_GL=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../../../LeapSDK/include -I ../../../../LeapSDK/util
  TARGET := MotionVisualizer
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

OBJECTS := \
  $(OBJDIR)/LeapUtil_8319e5cf.o \
  $(OBJDIR)/LeapUtilGL_4d7e95b4.o \
  $(OBJDIR)/Main_90ebc5c2.o \
  $(OBJDIR)/juce_core_f7efbdf.o \
  $(OBJDIR)/juce_data_structures_6c162e5b.o \
  $(OBJDIR)/juce_events_1b6260d3.o \
  $(OBJDIR)/juce_graphics_be749eb7.o \
  $(OBJDIR)/juce_gui_basics_1e8bd2b3.o \
  $(OBJDIR)/juce_gui_extra_171914fb.o \
  $(OBJDIR)/juce_opengl_dbaa88ff.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking MotionVisualizer
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning MotionVisualizer
	-@rm -f $(OUTDIR)/$(TARGET)
	-@rm -rf $(OBJDIR)/*
	-@rm -rf $(OBJDIR)

$(OBJDIR)/LeapUtil_8319e5cf.o: ../../../../LeapSDK/util/LeapUtil.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LeapUtil.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LeapUtilGL_4d7e95b4.o: ../../../../LeapSDK/util/LeapUtilGL.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LeapUtilGL.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Main.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_core_f7efbdf.o: ../../../ThirdParty/JUCE/modules/juce_core/juce_core.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_core.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_data_structures_6c162e5b.o: ../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_data_structures.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_events_1b6260d3.o: ../../../ThirdParty/JUCE/modules/juce_events/juce_events.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_events.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_graphics_be749eb7.o: ../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_graphics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_basics_1e8bd2b3.o: ../../../ThirdParty/JUCE/modules/juce_gui_basics/juce_gui_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_extra_171914fb.o: ../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_extra.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_opengl_dbaa88ff.o: ../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_opengl.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
