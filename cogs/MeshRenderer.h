#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "Entity.h"
#include "Model.h"
#include "Material.h"

namespace cogs
{
		namespace ecs
		{
				class Camera;

				class MeshRenderer : public Component
				{
				public:
						MeshRenderer(std::unique_ptr<graphics::Model> _model, std::unique_ptr<graphics::Material> _material);
						~MeshRenderer();

						/**
						* The initialize component function
						*/
						void init() override;

						/**
						* The update component function
						*/
						void update(float _deltaTime) override;

						/**
						* The render component function
						*/
						void render() override;

						const graphics::Material* getMaterial() const noexcept { return m_material.get(); }
						const graphics::Model* getModel()					  const noexcept { return m_model.get(); }

						void setMaterial(std::unique_ptr<graphics::Material> _material) { m_material = std::move(_material); }
						void setModel(std::unique_ptr<graphics::Model> _model)									 { m_model			 = std::move(_model); }

				private:
						Camera* m_camera;
						std::unique_ptr<graphics::Model> m_model;
						std::unique_ptr<graphics::Material> m_material;
				};
		}
}
#endif // !MESH_RENDERER_H