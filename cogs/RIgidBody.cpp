#include "RigidBody.h"

#include "Entity.h"

#include "CMotionState.h"
#include "Physics.h"

#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "ConeCollider.h"
#include "CylinderCollider.h"

#include <glm\gtc\type_ptr.hpp>

namespace cogs
{
		RigidBody::RigidBody(std::weak_ptr<Physics> _physicsWorld, float _mass, bool _isKinematic) :
				m_mass(_mass), m_isKinematic(_isKinematic), m_physicsWorld(_physicsWorld)
		{
				btClamp(m_mass, 0.0f, 1.0f);
		}

		RigidBody::~RigidBody()
		{
				m_physicsWorld.lock()->removeRigidBody(m_rigidBody.get());
		}

		void RigidBody::init()
		{
				m_motionState = std::make_shared<CMotionState>(m_entity.lock()->getComponent<Transform>());

				btVector3 intertia(0.0f, 0.0f, 0.0f);

				std::weak_ptr<Collider> colliderShape;

				if (m_entity.lock()->hasComponent<BoxCollider>())
				{
						colliderShape = m_entity.lock()->getComponent<BoxCollider>();
				}
				else if (m_entity.lock()->hasComponent<SphereCollider>())
				{
						colliderShape = m_entity.lock()->getComponent<SphereCollider>();
				}
				else if (m_entity.lock()->hasComponent<CapsuleCollider>())
				{
						colliderShape = m_entity.lock()->getComponent<CapsuleCollider>();
				}
				else if (m_entity.lock()->hasComponent<ConeCollider>())
				{
						colliderShape = m_entity.lock()->getComponent<ConeCollider>();
				}
				else if (m_entity.lock()->hasComponent<CylinderCollider>())
				{
						colliderShape = m_entity.lock()->getComponent<CylinderCollider>();
				}

				if (m_mass != 0.0f)
				{
						colliderShape.lock()->getShape().lock()->calculateLocalInertia(m_mass, intertia);
				}

				btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(m_mass, m_motionState.get(), colliderShape.lock()->getShape().lock().get(), intertia);

				m_rigidBody = std::make_shared<btRigidBody>(rigidBodyCI);

				if (m_isKinematic)
				{
						m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
				}

				m_physicsWorld.lock()->addRigidBody(m_rigidBody.get());

				m_rigidBody->setUserPointer(m_entity.lock().get());
		}

		void RigidBody::update(float _deltaTime)
		{
		}

		void RigidBody::translate(const glm::vec3 & _offset)
		{
				m_rigidBody->translate(btVector3(_offset.x, _offset.y, _offset.z));
		}
		void RigidBody::applyCentralForce(const glm::vec3 & _force)
		{
				m_rigidBody->applyCentralForce(btVector3(_force.x, _force.y, _force.z));
		}
		void RigidBody::applyForce(const glm::vec3 & _force, const glm::vec3 & _relativePos)
		{
				m_rigidBody->applyForce(btVector3(_force.x, _force.y, _force.z), btVector3(_relativePos.x, _relativePos.y, _relativePos.z));
		}
		void RigidBody::setLinearVelocity(const glm::vec3 & _offset)
		{
				m_rigidBody->setLinearVelocity(btVector3(_offset.x, _offset.y, _offset.z));
		}
		void RigidBody::setAngularVelocity(const glm::vec3 & _offset)
		{
				m_rigidBody->setAngularVelocity(btVector3(_offset.x, _offset.y, _offset.z));
		}
		void RigidBody::setRestitution(float _restitution)
		{
				m_rigidBody->setRestitution(_restitution);
		}
		void RigidBody::setFriction(float _friction)
		{
				m_rigidBody->setFriction(_friction);
		}
		void RigidBody::setLinearFactor(const glm::vec3 & _offset)
		{
				m_rigidBody->setLinearFactor(btVector3(_offset.x, _offset.y, _offset.z));
		}
		void RigidBody::setAngularFactor(const glm::vec3 & _offset)
		{
				m_rigidBody->setAngularFactor(btVector3(_offset.x, _offset.y, _offset.z));
		}
		void RigidBody::setDamping(float _x, float _y)
		{
				m_rigidBody->setDamping(_x, _y);
		}
		void RigidBody::setWorldTransform()
		{
				btTransform temp;
				temp.setFromOpenGLMatrix(glm::value_ptr(m_entity.lock()->getComponent<Transform>().lock()->worldTransform()));
				m_rigidBody->setWorldTransform(temp);
		}
		void RigidBody::activate()
		{
				m_rigidBody->activate();
		}
		void RigidBody::setActivationState(int _state)
		{
				m_rigidBody->forceActivationState(_state);
		}
}