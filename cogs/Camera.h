#ifndef CAMERA_H
#define CAMERA_H

#include "Transform.h"
#include "Framebuffer.h"
#include "Color.h"
#include "Skybox.h"

#include <vector>

namespace cogs
{
		namespace ecs
		{
				/**
				  * Projection types for the camera
				  */
				enum class ProjectionType
				{
						ORTHOGRAPHIC,
						PERSPECTIVE
				};

				/**
						* The camera component
						*/
				class Camera : public Component
				{
				public:
						/**
								*	\brief Constructor for the camera component
								* \param _projType - the projection type of the camera
								* \param _screenWidth - screen width
								* \param _screenHeight - screen height
								*/
						Camera(int _screenWidth, int _screenHeight, const ProjectionType& _projType);
						~Camera();

						void init() override;
						void update(float _deltaTime) override;

						/**
								* \brief Changes the FoV of the perspective matrix and updates it
								* \param _value - the new FoV value
								*/
						void setFoV(int _value);

						/**
						* \brief Offsets the FoV of the perspecive matrix and updates it
						* \param _value - the value to be added to the current FoV
						*/
						void offsetFoV(int _value);

						/**
								* \brief Changes the size of the orthographic matrix and updates it
								* \param _value - the new size value
								*/
						void setSize(float _value);

						/**
								* \brief Offsets the size of the orthographic matrix and updates it
								* \param _value - the value to be added to the current size
								*/
						void offsetSize(float _value);

						/**
						* \brief Changes the near plane
						* \param _value - the new near value
						*/
						void setNearPlane(float _value);

						/**
						* \brief Changes the far plane
						* \param _value - the new far value
						*/
						void setFarPlane(float _value);

						/**
						* \brief Sets the projection type of the matrix
						*/
						void setProjectionType(const ProjectionType& _projType) { m_projType = _projType; }

						/**
								* \brief Sets the screen width and height for the camera and updates the proj matrix
								* \param _screenWidth - the new width
								* \param _screenHeight - the new height
								*/
						void resize(int _screenWidth, int _screenHeight);

						/**
								* \brief Gets the aspect ratio of the screen
								* \return the division of SW / SH
								*/
						float getAspectRatio() const noexcept { return (float)m_cameraWidth / (float)m_cameraHeight; }

						/**
								* \brief Gets the projection matrix of the camera
								* \return the projection matrix
								*/
						const glm::mat4& getProjectionMatrix() const noexcept;

						/**
								* \brief Gets the view matrix of the camera
								* \return the view matrix
								*/
						const glm::mat4& getViewMatrix() const noexcept { return m_viewMatrix; }

						/**
						* \brief get and set the target framebuffer this camera renders to
						*/
						void setRenderTarget(std::weak_ptr<graphics::Framebuffer> _renderTarget) { m_renderTarget = _renderTarget; }
						std::weak_ptr<graphics::Framebuffer> getRenderTarget() { return m_renderTarget; }

						/**
						* \brief get and set the skybox this camera renders
						*/
						void setSkybox(std::weak_ptr<graphics::Skybox> _skybox) { m_skybox = _skybox; }
						std::weak_ptr<graphics::Skybox> getSkybox() { return m_skybox; }
						void renderSkybox();

						/**
						* \brief get and set the background color
						*/
						void setBackgroundColor(const graphics::Color& _color) { m_backgroundColor = _color; }
						const graphics::Color& getBackgroundColor() const noexcept { return m_backgroundColor; }

						/**
								* Some basic getters
								*/
						float getSize() const noexcept { return m_size; }
						float getNear()	const noexcept { return m_nearPlane; }
						float getFar()		const noexcept { return m_farPlane;; }
						int getFoV()				const noexcept { return m_fov; }
						int getWidth()		const noexcept { return m_cameraWidth; }
						int getHeight() const noexcept { return m_cameraHeight; }

						static void setMain(std::weak_ptr<Camera> _camera)    { s_mainCamera = _camera; }
						static void setCurrent(std::weak_ptr<Camera> _camera) { s_currentCamera = _camera; }
						static void addCamera(std::weak_ptr<Camera> _camera)  { s_allCameras.push_back(_camera); }

						static std::weak_ptr<Camera> getMain()                    { return s_mainCamera; }
						static std::weak_ptr<Camera> getCurrent()                 { return s_currentCamera; }
						static std::vector<std::weak_ptr<Camera>> getAllCameras() { return s_allCameras; }
				private:
						static std::weak_ptr<Camera> s_mainCamera;
						static std::weak_ptr<Camera> s_currentCamera;
						static std::vector<std::weak_ptr<Camera>> s_allCameras;

						ProjectionType m_projType{ ProjectionType::ORTHOGRAPHIC }; ///< the projection type of the camera

						glm::mat4 m_orthoMatrix{ 1.0f }; ///< orthographic matrix for ortho camera
						glm::mat4 m_perspMatrix{ 1.0f }; ///< perspective matrix for perspective camera
						glm::mat4 m_viewMatrix	{ 1.0f }; ///< Camera view matrix

						std::weak_ptr<Transform> m_transform; ///< the transform of the camera
						Transform m_oldTransform;										///< the old transform (to be compared for view changes

						float m_size{ 5.0f }; ///< the size of the ortho camera (zoom)
						int m_fov{ 60 };				  ///< the field of view of the perspective camera

						int m_cameraWidth{ 1024 }; ///< The width of the sdl screen
						int m_cameraHeight{ 576 }; ///< The height of the sdl screen

						float m_nearPlane{ 0.1f };  ///< the near clipping plane
						float m_farPlane{ 100.0f }; ///< the far clipping plane

						std::weak_ptr<graphics::Framebuffer> m_renderTarget;
						std::weak_ptr<graphics::Skybox> m_skybox;

						graphics::Color m_backgroundColor{ graphics::Color::white };
				};
		}
}
#endif // !CAMERA_H