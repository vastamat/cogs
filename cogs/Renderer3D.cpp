#include "Renderer3D.h"

#include "Camera.h"
#include "GLSLProgram.h"
#include "MeshRenderer.h"

namespace cogs
{
		namespace graphics
		{
				Renderer3D::Renderer3D(std::weak_ptr<GLSLProgram> _shader) : m_shader(_shader)
				{
				}
				Renderer3D::Renderer3D()
				{
				}
				Renderer3D::~Renderer3D()
				{
				}
				void Renderer3D::init()
				{
						
				}
				void Renderer3D::submit(std::weak_ptr<ecs::Entity> _entity)
				{
						m_entities.push_back(_entity);
				}
				void Renderer3D::flush()
				{
						//get the current cam that will be used for space-transforms
						std::weak_ptr<ecs::Camera> currentCam = ecs::Camera::getCurrent();

						//begind using the shader this renderer uses
						m_shader.lock()->use();
						//upload the projection and view matrices as they are the same for every entity in this render queue
						m_shader.lock()->uploadValue("projection", currentCam.lock()->getProjectionMatrix());
						m_shader.lock()->uploadValue("view", currentCam.lock()->getViewMatrix());

						for (std::weak_ptr<ecs::Entity> entity : m_entities)
						{
								std::weak_ptr<Model> model = entity.lock()->getComponent<ecs::MeshRenderer>().lock()->getModel();
								std::weak_ptr<Material> material = entity.lock()->getComponent<ecs::MeshRenderer>().lock()->getMaterial();

								//upload the model matrix as it's the same for 1 whole entity
								m_shader.lock()->uploadValue("model", entity.lock()->getComponent<ecs::Transform>().lock()->worldTransform());
								//Upload the material uniform values (vec2's/3's, floats, mat4's)
								m_shader.lock()->uploadMaterial(material);

								for (auto& mesh : model.lock()->getMeshes())
								{
										//upload the per-mesh textures
										m_shader.lock()->uploadMeshTextures(mesh.getTextures());

										//draw the mesh
										mesh.render();
								}
						}

						//finally unbind the current shader program
						m_shader.lock()->unUse();
				}
				void Renderer3D::begin()
				{
						m_entities.clear();
				}
				void Renderer3D::end()
				{
						//TODO: possibly sorting and batching/preparing for instanced rendering ?
				}
				void Renderer3D::setShader(std::weak_ptr<GLSLProgram> _shader)
				{
						m_shader = _shader;
				}
		}
}