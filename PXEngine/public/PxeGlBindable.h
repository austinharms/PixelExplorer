#ifndef PXENGINE_GL_BINDABLE_H_
#define PXENGINE_GL_BINDABLE_H_
namespace pxengine {
	// Interface class to allow PxeGlAssets that bind to OpenGl state to share a common class
	class PxeGlBindable
	{
	public:
		PxeGlBindable() = default;
		virtual ~PxeGlBindable() = default;
	
		// This method should bind the OpenGl object to the current state
		// Note: this function MUST be called from the render thread 
		virtual void bind() = 0;

		// This method should remove all bound OpenGl objects from state that were bound in bind()
		// Note: this function MUST be called from the render thread 
		virtual void unbind() = 0;
	};
}
#endif // !PXENGINE_BINDABLE_H_
