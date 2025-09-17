#include "../../include/geometry/mesh.h"


// Constructor
Mesh::Mesh(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices) {
    Mesh::vertices = vertices;
    Mesh::indices = indices;

    //Generate Vertex Array Object and binds it
    vao.Bind();
    //Generates Vertex Buffer Object and links it to vertices
    VBO vbo(vertices);
    //Generate Element Buffer Object and links it to indices
    EBO ebo(indices);
    //Link VBO attributes such as coordinates and colors to VAO
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), nullptr);
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    vao.LinkAttrib(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    //Unbind all to prevent accidentally modifying them
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();

}

void Mesh::ApplyTransformations() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    modelMatrix = glm::scale(modelMatrix, scale);
}

void Mesh::Draw(Shader& shader, Camera& camera) {
    GLuint modelLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    GLuint normalLoc = glGetUniformLocation(shader.ID, "normalMatrix");
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));


    shader.Activate();

    if (useTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texId);
        //Set the sampler uniform to use texture unit 0
        GLint texLoc = glGetUniformLocation(shader.ID, "tex0");
        glUniform1i(texLoc, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specMapId);
        //Set the sampler uniform to use texture unit 1
        GLint specMapLoc = glGetUniformLocation(shader.ID, "tex1");
        glUniform1i(specMapLoc, 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMapId);
        //Set the sampler uniform to use texture unit 2
        GLint normalMapLoc = glGetUniformLocation(shader.ID, "normal0");
        glUniform1i(normalMapLoc, 1);
    }

    vao.Bind();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
void Mesh::setupBuffers() {
    vao.Bind();
    VBO vbo(vertices);
    EBO ebo(indices);
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 8*sizeof(float), nullptr);
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, 8*sizeof(float), reinterpret_cast<void*>(3*sizeof(float)));
    vao.LinkAttrib(vbo, 2, 2, GL_FLOAT, 8*sizeof(float), reinterpret_cast<void*>(6*sizeof(float)));
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

