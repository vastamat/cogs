#ifndef ENTITY_H
#define ENTITY_H

#include "Object.h"
#include "Transform.h"

#include <bitset>
#include <cassert>
#include <vector>
#include <array>

namespace cogs
{
		constexpr std::size_t MAX_COMPONENTS{ 32 };

		/**
		* \brief The Entity class, which is a collection of components
		*/
		class Entity : public Object, public std::enable_shared_from_this<Entity>
		{
		public:
				/** The constructor adds a transform component, as all entities need at least a transform */
				Entity()
				{
				}

				/** The constructor adds a transform component, as all entities need at least a transform */
				Entity(const std::string& _name) : Object(_name)
				{
				}
				~Entity()
				{
				}

				/**
				* \brief Creates an entity and adds a transform component to it,
				* as every entity will have at least a transform
				*/
				static std::shared_ptr<Entity> create(const std::string& _name)
				{
						//create a new entity shared ptr
						std::shared_ptr<Entity> newEntity = std::make_shared<Entity>(_name);
						//give it a transform
						newEntity->addComponent<Transform>();
						//return the created entity
						return std::move(newEntity);
				}

				/** Updates this entity and all its children */
				inline void updateAll(float _deltaTime)
				{
						if (m_isActive)
						{
								update(_deltaTime);
								for (auto& child : m_children)
								{
										child->updateAll(_deltaTime);
								}
						}
				}

				/** Renders this entity and all its children */
				inline void renderAll()
				{
						if (m_isActive)
						{
								render();
								for (auto& child : m_children)
								{
										child->renderAll();
								}
						}
				}

				/** Renders calls the postprocess function of this entity and all its children */
				inline void postProcessAll()
				{
						if (m_isActive)
						{
								postProcess();
								for (auto& child : m_children)
								{
										child->postProcessAll();
								}
						}
				}

				/**
				* \brief refreshes the children vector, deleting all children set with destroyed flag to true
				* Then it calls the refresh function of all alive children, so that they can delete their destroyed children
				*/
				inline void refreshAll()
				{
						refresh();
						for (auto& child : m_children)
						{
								child->refreshAll();
						}
				}

				/**
				* \brief This is called by the physics engine when the entity collides with something
				* it calls the onCollision function of all of its components for collision handling
				*/
				inline void collide(const glm::vec3& _pointA,
						const glm::vec3& _pointB,
						const glm::vec3& _normalOnB,
						Entity* _other)
				{
						for (auto& component : m_components)
						{
								component->onCollision(_pointA, _pointB, _normalOnB, _other);
						}
				}

				/**
				* \brief Add components to this element of any type
				* \param[in] T is the component type
				* \param[in] TArgs is a parameter pack of types used to construct the component
				*/
				template<typename T, typename... TArgs>
				inline void addComponent(TArgs&&... _args)
				{
						/* check if this component is not already added */
						assert(!hasComponent<T>());

						/* begin by allocating component of type T on the heap by forwarding the passed arguments to its constructor */
						std::shared_ptr<Component> component = std::make_shared<T>(std::forward<TArgs>(_args)...);
						component->setEntity(shared_from_this());

						/* Add the component to the vector (std::move is required as unique pointers cannot be copied) */
						m_components.emplace_back(std::move(component));

						/* when a component of type T is added, add it to the bitset and the array */
						m_componentArray[getComponentTypeID<T>()] = m_components.back();
						m_componentBitset[getComponentTypeID<T>()] = true;

						/* Call the virtual function init of the component */
						m_components.back()->init();
				}

				/**
				* \brief Adds a child to the vector of children
				* \param[in] _name The name of the child
				* \param[out] std::weak_ptr<Entity> return a weak pointer to the new entity
				*/
				std::weak_ptr<Entity> addChild(const std::string& _name)
				{
						/* create the new entity shared pointer */
						std::shared_ptr<Entity> newChild = create(_name);
						/* move it to the children vector */
						m_children.push_back(std::move(newChild));
						/* set the transform parent of the child entity to this entity's transform */
						m_children.back()->getComponent<Transform>().lock()->setParent(getComponent<Transform>());
						/* return a handle(reference) of the child */
						return m_children.back();
				}

				/**
				* \brief Adds an existing entity to the children vector of this entity
				*	move semantics are used internally so the main child pointer is moved to the vector (making the main one empty)
				*	after this the weak ptr handle should be used.
				* \param[in] _child the entity to be added as a child.
				* The && makes it so that std::move must be used when passing the entity
				*/
				std::weak_ptr<Entity> addChild(std::shared_ptr<Entity>&& _child)
				{
						/* move it to the children vector */
						m_children.push_back(std::move(_child));
						/* set the transform parent of the child entity to this entity's transform */
						m_children.back()->getComponent<Transform>().lock()->setParent(getComponent<Transform>());
						/* return a handle(reference) of the child */
						return m_children.back();
				}

				/**
				* \brief detaches a child from this entity and returns the independant child
				* \param[in] _childRef - weak pointer to the child to be detached
				* \param[out] std::shared_ptr<Entity> - the independent children with std::move
				*/
				std::shared_ptr<Entity> detachChild(std::weak_ptr<Entity> _childRef)
				{
						if (_childRef.expired())
						{
								return nullptr;
						}
						for (size_t i = 0; i < m_children.size(); i++)
						{
								if (m_children.at(i) == _childRef.lock())
								{
										//set the parent to null
										m_children.at(i)->getComponent<Transform>().lock()->setParent(std::weak_ptr<Transform>());
										//get the now independent child
										std::shared_ptr<Entity> independentChild = std::move(m_children.at(i));
										//erase the empty slot from the vector
										m_children.erase(m_children.begin() + i);
										//return the independent child as a main handle
										return std::move(independentChild);
								}
						}
						return nullptr;
				}

				/**
				* \brief detatches this child from its parent
				* \param[out] std::shared_ptr<Entity> - the independent children with std::move
				*/
				std::shared_ptr<Entity> detachFromParent()
				{
						std::weak_ptr<Transform> parent = getComponent<Transform>().lock()->getParent();
						if (parent.expired())
						{
								//nothing to detach from
						}
						else
						{
								return std::move(parent.lock()->getEntity().lock()->detachChild(getComponent<Transform>().lock()->getEntity()));
						}
				}

				/**
				* \brief Checks if a specific component exists in the current entity
				*/
				template<typename T>
				inline bool hasComponent() const
				{
						/* ask the bitset if the bit if the bit at the position with the type T is true or false */
						return m_componentBitset[getComponentTypeID<T>()];
				}

				/**
				* \brief get a specific component from the m_componentArray
				* \param[out] std::weak_ptr<T> return a reference to the component requested
				*/
				template<typename T>
				inline std::weak_ptr<T> getComponent() const
				{
						/* check if it has this component */
						assert(hasComponent<T>());
						//get the component pointer
						std::weak_ptr<Component> component = (m_componentArray[getComponentTypeID<T>()]);
						return std::static_pointer_cast<T>(component.lock());
				}

				/**
				* \brief Checks if any child of this entity has T component, and returns the first one found
				* if it doesn't exist, return null
				*/
				template<typename T>
				inline std::weak_ptr<T> getComponentInChildren() const
				{
						for (auto& child : m_children)
						{
								if (child->hasComponent<T>())
								{
										return child->getComponent<T>();
								}
						}
						return std::weak_ptr<T>();
				}

				/**
				* \brief checks if any child of this entity has T components, and returns a vector of all the components found
				*/
				template<typename T>
				inline std::vector<std::weak_ptr<T>> getComponentsInChildren() const
				{
						std::vector<std::weak_ptr<T>> components;
						for (auto& child : m_children)
						{
								if (child->hasComponent<T>())
								{
										components.push_back(child->getComponent<T>());
								}
						}
						return components;
				}

				/**
				* \brief Searches for the first child in the children vector with name _entityName and returns a reference to it
				*/
				inline std::weak_ptr<Entity> getChild(const std::string& _entityName)
				{
						for (size_t i = 0; i < m_children.size(); i++)
						{
								if (m_children.at(i)->getName() == _entityName)
								{
										return m_children.at(i);
								}
						}
						return std::weak_ptr<Entity>();
				}

				/**
				* \brief returns the child at index _index if there is one, returns null otherwise
				*/
				inline std::weak_ptr<Entity> getChild(unsigned int _index)
				{
						if (m_children.size() > _index)
						{
								return m_children.at(_index);
						}
						return std::weak_ptr<Entity>();
				}

				//tag setter
				void setTag(const std::string& _tag) { m_tag = _tag; }
				//tag getter
				const std::string& getTag() const noexcept { return m_tag; }

				//sets the active state of the entity
				void setActive(bool _active)
				{
						m_isActive = _active;
						/*if (!m_isActive)
						{
								for (size_t i = 0; i < m_children.size(); i++)
								{
										m_children.at(i)->setActive(false);
								}
						}*/
				}

				//gets the active state of the entity
				bool isActive() const noexcept { return m_isActive; }

		private:
				/* Update this entity (all its components) */
				inline void update(float _deltaTime) { for (auto& component : m_components) { component->update(_deltaTime); } }

				/* Render this entity (all its components) */
				inline void render() { for (auto& component : m_components) { component->render(); } }

				/* call the postprocess functions of this entity (all its components) */
				inline void postProcess() { for (auto& component : m_components) { component->postProcess(); } }

				/* Refresh this entity by deleting all children set to be destroyed */
				inline void refresh()
				{
						m_children.erase
						(
								std::remove_if(std::begin(m_children), std::end(m_children),
										[](const std::shared_ptr<Entity>& _child)
						{
								return _child->isDestroyed();
						}),
								std::end(m_children)
								);
				}

		private:
				/* The Entity's tag */
				std::string m_tag{ "default" };

				/* active flag of the entity */
				bool m_isActive{ true };

				/* An entity is also composed of numerous components
					* Therefore the components will be stored in an std::vector as shared pointers to allow polymorphism */
				std::vector<std::shared_ptr<Component>> m_components;

				/* The children of this entity */
				std::vector<std::shared_ptr<Entity>> m_children;

				/* A bitset to check the existance of a component with a specific ID */
				std::bitset<MAX_COMPONENTS> m_componentBitset;

				/* An array to get a component with specific ID */
				std::array<std::weak_ptr<Component>, MAX_COMPONENTS> m_componentArray;
		};
}
#endif // !ENTITY_H