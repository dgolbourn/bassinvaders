#include "scene.h"
#include <map>
#include "jansson.h"

namespace game
{
typedef std::multimap<int, Layer> LayerMap;
typedef std::pair<int, Layer> LayerPair;

class SceneImpl
{
public:
  SceneImpl(json::JSON const& json, display::Window& window);
  void Add(Layer const& layer, int z);
  void Render(void);
  LayerMap layers_;
};

void SceneImpl::Render(void)
{
  for(auto iter = layers_.begin(); iter != layers_.end();)
  {
    if(iter->second())
    {
      ++iter;
    }
    else
    {
      iter = layers_.erase(iter);
    }
  }
}

void SceneImpl::Add(Layer const& layer, int z)
{
  layers_.insert(LayerPair(z, layer));
}

SceneImpl::SceneImpl(json::JSON const& json, display::Window& window)
{
  json_t* layers;
  json.Unpack("{so}", "layers", &layers);
  size_t index;
  json_t* layer;
  json_array_foreach(layers, index, layer)
  {
    char const* filename;
    int plane;
    double parallax;
    json_t* render_box;
    json::JSON(layer).Unpack("{sssisfso}", 
      "image", &filename,
      "z", &plane,
      "parallax", &parallax,
      "render box", &render_box);

    Layer bind_layer = std::bind(
      window.Load(filename),
      display::BoundingBox(),
      display::BoundingBox(render_box),
      float(parallax),
      true,
      0);
    
    Add(bind_layer, plane);
  }
}

Scene::Scene(void)
{
}

void Scene::Render(void)
{
  impl_->Render();
}

void Scene::Add(Layer const& command, int z)
{
  impl_->Add(command, z);
}

Scene::Scene(std::string const& filename, display::Window& window) : impl_(new SceneImpl(json::JSON(filename), window))
{
}

Scene::Scene(json::JSON const& json, display::Window& window) : impl_(new SceneImpl(json, window))
{
}

Scene::Scene(Scene const& other) : impl_(other.impl_)
{
}

Scene::Scene(Scene&& other) : impl_(std::move(other.impl_))
{
}

Scene::~Scene(void)
{
}

Scene& Scene::operator=(Scene other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}