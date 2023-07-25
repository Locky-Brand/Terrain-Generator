#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./vector3D_gfx/vector3D_gfx.h"
#include "./lighting3D_gfx/lighting3D_gfx.h"

#include "./terrain3D_gfx/terrain3D_gfx.h"

int main(void) {
  vector3D_gfx box = init("light trial", 800, 800, 3, 3, 0, NULL, (vec4){0.2f, 0.5f, 0.7f, 1.0f});

  lighting3D_gfx lights = {(vec4){0.2f, 0.2f, 0.2f, 1.0f}, 0, NULL};

  perlin_dbg_tex te;
  terrain_mesh m = create_terrain_plane(&box, 1.0f, 120, 120, 10.0f, 8, 0.5f, 2.0f, &te);

  terrain_mesh w = create_flat_plane(&box, 100.0f, 13, 13, 25.0f, (vec4){0.0f, 0.0f, 1.0f, 1.0f});

  set_ambient_light(&box, (vec4){0.2f, 0.2f, 0.2f, 1.0f});

  point_light w_light_test = create_point_light(&box, &lights, (vec3){1.0f, 20.0f, 0.0f}, (vec3){1.0f, 1.0f, 1.0f}, 0.5f);

  while(!close_flag(&box)) {
    frame_begin(&box);

    get_camera3D_input(&box, NULL);

    update_lighting(&box, &lights);

    move_light_to(&box, &w_light_test, box.cameras[box.cam_index]->cam_pos);

    render(&box);

    draw_perlin_dbg_tex(te);

    frame_end(&box);
  }

  free(m.verts);

  destroy(&box);

  return 0;
}
