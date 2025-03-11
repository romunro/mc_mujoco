#pragma once

#include "details/TransformBase.h"

namespace mc_mujoco
{

struct Rotation : public TransformBase<ControlAxis::ROTATION>
{
  Rotation(Client & client, const ElementId & id, const ElementId & reqId) : TransformBase(client, id, reqId) {}

  void draw3D() override
  {
    if(!show_)
    {
      return;
    }

    TransformBase::draw3D();
    mclient_.draw_frame(marker_.pose());
  }

  void draw2D() override
  {
    ImGui::Checkbox(label(fmt::format("Show {}", id.name)).c_str(), &show_);
  }

private:
  bool show_ = false;
};

} // namespace mc_mujoco
