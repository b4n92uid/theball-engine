/*
 * File:   NewtonNode.h
 * Author: b4n92uid
 *
 * Created on 27 novembre 2009, 19:35
 */

#ifndef _NEWTONNODE_H
#define _NEWTONNODE_H

#include <Newton.h>

#include "Vector3.h"
#include "Matrix4.h"

namespace tbe {
    namespace scene {

        class NewtonParallelScene;

        /// \brief Class represantant un objet physique dans l'espace

        class NewtonNode : public Node {
        public:
            NewtonNode(NewtonParallelScene* newtonScene);
            NewtonNode(NewtonParallelScene* newtonScene, Node* node);
            NewtonNode(const NewtonNode& copy);

            virtual ~NewtonNode();

            bool operator=(const NewtonNode& copy);

            void buildBoxNode(Vector3f size, float masse);
            void buildSphereNode(Vector3f size, float masse);
            void buildCylinderNode(Vector3f size, float masse);
            void buildConvexNode(const tbe::scene::Mesh* mesh, float masse);
            void buildConvexNode(const Vertex::Array& vertexes, float masse, Matrix4 offset = Matrix4());
            void buildTreeNode(const tbe::scene::Mesh* mesh);
            void buildTreeNode(const Face::Array& faces);

            /// Destruction de l'objet physique
            void destroyBody();

            /// Recuperation Objet NewtonBody
            NewtonBody* getBody() const;

            /// Test de collision
            bool isCollidWith(const NewtonNode* target) const;

            /// Masse
            float getMasse();

            /// Assigne une matrice
            void setMatrix(Matrix4 matrix);
            Matrix4 getMatrix();

            void setPos(Vector3f pos);
            Vector3f getPos();

            void setVelocity(Vector3f vel);
            Vector3f getVelocity();

            void setOmega(Vector3f vel);
            Vector3f getOmega();

            void render();

            void process();

            Node* clone();

            void setApplyTorque(Vector3f applyTorque);
            Vector3f getApplyTorque() const;

            void setApplyForce(Vector3f applyForce);
            Vector3f getApplyForce() const;

            void setApplyGravity(bool applyGravity);
            bool isApplyGravity() const;

            void applyForceAndTorque();

            NewtonParallelScene* getParallelScene() const;

            /// Method static d'application de la force gravitationnele (9.81)
            static void applyForceAndTorqueCallback(const NewtonBody* body, float, int);

            /// Method static d'application de la transformation
            static void applyTransformCallback(const NewtonBody* body, const float*, int);

            typedef std::vector<NewtonNode*> Array;
            typedef std::map<std::string, NewtonNode*> Map;

        protected:
            Vector3f m_applyForce;
            Vector3f m_applyTorque;
            NewtonParallelScene* m_parallelScene;
            NewtonWorld* m_newtonWorld;
            NewtonBody* m_body;
            Node* m_visualBody;
            float m_masse;
            bool m_applyGravity;
        };

    }
}

#endif /* _NEWTONNODE_H */

