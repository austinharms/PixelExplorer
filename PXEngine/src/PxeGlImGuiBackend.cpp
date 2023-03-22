#include "PxeGlImGuiBackend.h"

#include "PxeEngine.h"
#include "PxeLogger.h"
#include "imgui.h"
#include "GL/glew.h"

namespace pxengine {
	PxeGlImGuiBackend::PxeGlImGuiBackend() {
		_guiShader = PxeEngine::getInstance().getRenderPipeline().loadShader<PxeGuiShader>();
		_glVertexArray = 0;
		_glArrayBuffer = 0;
		_glElementBuffer = 0;
		_glFontTextureId = 0;
	}

	PxeGlImGuiBackend::~PxeGlImGuiBackend() {
		if (_guiShader)_guiShader->drop();
	}

	// TODO Make this return if the backend was installed
	void PxeGlImGuiBackend::installBackend()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.BackendRendererUserData) {
			PXE_ERROR("Attempted to initialize already initialized gui renderer backend");
			return;
		}

		io.BackendRendererUserData = (void*)this;
		io.BackendRendererName = "PXENGINE_NpGuiGlBackend";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	}

	void PxeGlImGuiBackend::uninstallBackend()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (!io.BackendRendererUserData) {
			PXE_ERROR("Attempted to shutdown invalid gui renderer backend");
			return;
		}
		else if (io.BackendRendererUserData != this) {
			PXE_ERROR("Attempted to shutdown incorrect gui renderer backend");
			return;
		}

		io.BackendRendererName = nullptr;
		io.BackendRendererUserData = nullptr;
	}

	void PxeGlImGuiBackend::rebuildFontTexture()
	{
		ImGuiIO& io = ImGui::GetIO();
		uint8_t* pixels;
		int32_t width;
		int32_t height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		glBindTexture(GL_TEXTURE_2D, _glFontTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		io.Fonts->SetTexID((ImTextureID)(intptr_t)_glFontTextureId);
	}

	void PxeGlImGuiBackend::renderDrawData()
	{
		if (getAssetStatus() != PxeGLAssetStatus::INITIALIZED) {
			PXE_FATAL("Attempted to render gui using non initialized render backend");
		}

		ImDrawData* drawData = ImGui::GetDrawData();
		int32_t fbWidth = (int32_t)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
		int32_t fbHeight = (int32_t)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
		if (fbWidth <= 0 || fbHeight <= 0) return;
		glViewport(0, 0, fbWidth, fbHeight);
		_guiShader->setProjectionMatrix(glm::ortho(drawData->DisplayPos.x, drawData->DisplayPos.x + drawData->DisplaySize.x, drawData->DisplayPos.y + drawData->DisplaySize.y, drawData->DisplayPos.y));
		_guiShader->setTexture(0);
		glBindVertexArray(_glVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, _glArrayBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glElementBuffer);

		// Will project scissor/clipping rectangles into framebuffer space
		ImVec2 clip_off = drawData->DisplayPos;         // (0,0) unless using multi-viewports
		ImVec2 clip_scale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* cmdList = drawData->CmdLists[n];
			glBufferData(GL_ARRAY_BUFFER, (size_t)cmdList->VtxBuffer.Size * sizeof(ImDrawVert), (const void*)cmdList->VtxBuffer.Data, GL_STREAM_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (size_t)cmdList->IdxBuffer.Size * sizeof(ImDrawIdx), (const void*)cmdList->IdxBuffer.Data, GL_STREAM_DRAW);
			for (int cmdIndex = 0; cmdIndex < cmdList->CmdBuffer.Size; cmdIndex++)
			{
				const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmdIndex];
				if (pcmd->UserCallback != nullptr)
				{
					if (pcmd->UserCallback == ImDrawCallback_ResetRenderState) {
						// TODO Implement this
						PXE_ERROR("Attempted to reset ImGui render state");
					}
					else {
						pcmd->UserCallback(cmdList, pcmd);
					}
				}
				else
				{
					ImVec2 clipMin((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
					ImVec2 clipMax((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
					if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
						continue;
					glScissor((int)clipMin.x, (int)((float)fbHeight - clipMax.y), (int)(clipMax.x - clipMin.x), (int)(clipMax.y - clipMin.y));
					_guiShader->setTexture((uint32_t)(pcmd->GetTexID()));
					glDrawElementsBaseVertex(GL_TRIANGLES, (int32_t)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (int32_t)pcmd->VtxOffset);
				}
			}
		}

		glBindVertexArray(0);
	}

	void PxeGlImGuiBackend::initializeGl()
	{
		int32_t previousTexture;
		int32_t previousVertexArray;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVertexArray);
		glGenVertexArrays(1, &_glVertexArray);
		glBindVertexArray(_glVertexArray);
		glGenBuffers(1, &_glArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _glArrayBuffer);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, (const void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (const void*)8);
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 20, (const void*)16);
		glGenBuffers(1, &_glElementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _glElementBuffer);
		glGenTextures(1, &_glFontTextureId);
		rebuildFontTexture();
		glBindVertexArray(previousVertexArray);
		glBindTexture(GL_TEXTURE_2D, previousTexture);
		PXE_INFO("PxeGlImGuiBackend initialized");
	}

	void PxeGlImGuiBackend::uninitializeGl()
	{
		glDeleteBuffers(1, &_glArrayBuffer);
		glDeleteBuffers(1, &_glElementBuffer);
		glDeleteVertexArrays(1, &_glVertexArray);
		glDeleteTextures(1, &_glFontTextureId);
		PXE_INFO("PxeGlImGuiBackend uninitialized");
	}
}
