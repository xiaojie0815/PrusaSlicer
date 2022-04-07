#include "../libslic3r.h"
#include "../Model.hpp"
#include "../TriangleMesh.hpp"

#include "STL.hpp"

#include <string>

#ifdef _WIN32
#define DIR_SEPARATOR '\\'
#else
#define DIR_SEPARATOR '/'
#endif

namespace Slic3r {

bool load_stl(const char *path, Model *model, const char *object_name_in)
{
    BOOST_LOG_TRIVIAL(trace) << "load_stl() starts...";
    TriangleMesh mesh;
    if (! mesh.ReadSTLFile(path)) {
        BOOST_LOG_TRIVIAL(trace) << "load_stl(): ReadSTLFile returned false";
        return false;
    }
    if (mesh.empty()) {
        BOOST_LOG_TRIVIAL(trace) << "load_stl(): mesh loaded but seems to be empty";
        return false;
    }

    std::string object_name;
    if (object_name_in == nullptr) {
        const char *last_slash = strrchr(path, DIR_SEPARATOR);
        object_name.assign((last_slash == nullptr) ? path : last_slash + 1);
    } else
       object_name.assign(object_name_in);

    model->add_object(object_name.c_str(), path, std::move(mesh));
    BOOST_LOG_TRIVIAL(trace) << "load_stl() about to return true...";
    return true;
}

bool store_stl(const char *path, TriangleMesh *mesh, bool binary)
{
    if (binary)
        mesh->write_binary(path);
    else
        mesh->write_ascii(path);
    //FIXME returning false even if write failed.
    return true;
}

bool store_stl(const char *path, ModelObject *model_object, bool binary)
{
    TriangleMesh mesh = model_object->mesh();
    return store_stl(path, &mesh, binary);
}

bool store_stl(const char *path, Model *model, bool binary)
{
    TriangleMesh mesh = model->mesh();
    return store_stl(path, &mesh, binary);
}

}; // namespace Slic3r
