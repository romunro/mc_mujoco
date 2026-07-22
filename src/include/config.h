#pragma once

namespace mc_mujoco
{

/** Default key path */
constexpr auto MUJOCO_KEY_PATH = "/home/thibault/.mujoco/mujoco-3.3.6/bin/mjkey.txt";

/** System folder searched for Mujoco models */
constexpr auto SHARE_FOLDER = "/usr/local/share/mc_mujoco";

/** User folder searched for Mujoco models */
constexpr auto USER_FOLDER = "/home/thibault/.config/mc_rtc/mc_mujoco";

} // namespace mc_mujoco
