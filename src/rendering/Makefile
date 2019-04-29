BASE_IMPL             	= $(OBJDIR)/base_backend.o

GLX_IMPL                = $(OBJDIR)/glx/backend.o $(OBJDIR)/glx/factory.o
GLX_LIBS				= $(OPENGL_LIBS)
GLX_HEADERS				= $(OPENGL_HEADERS)

FILE                    = $(@:$(OBJDIR)/%.o=%.cpp)

.PHONY: all target

target: all

all: $(LIB_R3D_GLX)

$(LIB_R3D_GLX): $(BASE_IMPL) $(GLX_IMPL)
	@echo "  $(CXX) $(notdir $(LIB_R3D_GLX))"
	@$(CXX) -o $(LIB_R3D_GLX) $(BASE_IMPL) $(GLX_IMPL) $(SO_FLAGS) $(XLIB_LIBS) $(OPENGL_LIBS)

$(BASE_IMPL):
	@echo "  $(CXX) $(FILE)"
	@$(CXX) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE)
	
$(GLX_IMPL):
	@echo "  $(CXX) $(FILE)"
	@mkdir -p $(dir $(@))
	@$(CXX) -o $(@) -c $(FILE) -fPIC $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) $(XLIB_HEADERS) $(OPENGL_HEADERS)
	
