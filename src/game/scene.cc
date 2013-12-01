#include "scene.h"
#include <map>
#include <vector>
#include "jansson.h"

namespace game
{
typedef std::multimap<int, event::CommandPtr> CommandMap;
typedef std::pair<int, event::CommandPtr> CommandPair;

class SceneImpl
{
public:
  SceneImpl(json::JSON const& json, display::Window& window);
  void Add(event::Command const& render, int z);
  void Render(void);
  CommandMap commands_;
  std::vector<event::Command> layers_;
};

void SceneImpl::Render(void)
{
  for(auto iter = commands_.begin(); iter != commands_.end();)
  {
    if(auto command = iter->second.lock())
    {
      command->operator()();
      ++iter;
    }
    else
    {
      iter = commands_.erase(iter);
    }
  }
}

void SceneImpl::Add(event::Command const& command, int z)
{
  commands_.insert(CommandPair(z, command));
}

class Layer : public event::CommandImpl
{
public:
  display::Texture texture_;
  display::BoundingBox render_box_;
  float parallax_;
  Layer(std::string const& filename, double parallax, display::Window& window, json::JSON const& render_box)
  {
    texture_ = window.Load(filename);
    parallax_ = float(parallax);
    render_box_ = display::BoundingBox(render_box);
  }
  void operator()(void) final
  {
    texture_.Render(display::BoundingBox(), render_box_, parallax_, true);
  }
};

SceneImpl::SceneImpl(json::JSON const& json, display::Window& window)
{
  json_t* layers;
  json.Unpack("{so}", 0, "layers", &layers);

  layers_ = std::vector<event::Command>(json_array_size(layers));
  size_t index;
  json_t* layer;
  json_array_foreach(layers, index, layer)
  {
    char const* filename = nullptr;
    int plane;
    double parallax;
    json_t* render_box;
    json::JSON(layer).Unpack("{sssisfso}", 0, 
      "filename", filename,
      "z", &plane,
      "parallax", &parallax,
      "render box", &render_box);

    event::Command command(new Layer(filename, parallax, window, render_box));
    layers_.push_back(command);
    Add(command, plane);
  }
}

Scene::Scene(void)
{
}

void Scene::Render(void)
{
  impl_->Render();
}

void Scene::Add(event::Command const& command, int z)
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