/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 11:04:08 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/09 18:40:54 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./library/minilibx-linux/mlx.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/keysym.h>
#include <math.h>

#define TILE_SIZE 20
#define PLAYER_SIZE 10
#define PI 3.1415926535
#define NUM_RAYS 60
#define ROT_SPEED 0.1
#define MOVE_SPEED 3.0
#define WINDOW_WIDTH 250
#define WINDOW_HEIGHT 150

typedef struct s_mlx_data
{
    void    *mlx;
    void    *window;
    void    *img;
    char    *addr;
    int     bpp;
    int     line_len;
    int     endian;
    int     columns;
    int     rows;
    char    **map;
    char    player;
    double  px;
    double  py;
    double  pa;
    double  fov;
    double  camera_x;
    double  camera_y;
}           t_mlx_data;

// map
char *map[] = {
    "11111111111111111111111111111",
    "10000000011111000000011110001",
    "10111000000011100000000000001",
    "10111111000011100001111000001",
    "10001000000000000000001111111",
    "10000000000000000000000000001",
    "10000000000000N00000000000001",
    "10000000000000000000000000001",
    "10000000011111000000011110001",
    "10111000000011100000000000001",
    "10111111000011100001111000001",
    "11111111111111111111111111111",
    NULL
};

static char **copy_map(char **map)
{
    int i = 0;
    char **new_map;
    while (map[i])
        i++;
    new_map = malloc(sizeof(char *) * (i + 1));
    if (!new_map)
        return NULL;
    for (int j = 0; j < i; j++)
        new_map[j] = strdup(map[j]);
    new_map[i] = NULL;
    return (new_map);
}

static void calc_rows_columns(t_mlx_data *data)
{
    data->rows = 0;
    data->columns = 0;
    while (data->map[data->rows])
        data->rows++;    
    data->columns = strlen(data->map[0]);
}

void find_Player(t_mlx_data *data)
{
    for (int x = 0; data->map[x]; x++)
    {
        for (int y = 0; data->map[x][y]; y++)
        {
            if (data->map[x][y] == 'N' || data->map[x][y] == 'S' || data->map[x][y] == 'E' || data->map[x][y] == 'W')
            {
                data->player = data->map[x][y];
                data->px = y * TILE_SIZE + TILE_SIZE / 2;
                data->py = x * TILE_SIZE + TILE_SIZE / 2;
                data->map[x][y] = '0';
                if (data->player == 'N')
                    data->pa = -PI / 2;
                else if (data->player == 'S')
                    data->pa = PI / 2;
                else if (data->player == 'W')
                    data->pa = PI;
                else if (data->player == 'E') 
                    data->pa = 0;
            }
        }
    }
}

static int close_window()
{
    exit(0);
}

static void update_camera(t_mlx_data *data)
{
    data->camera_x = data->px - WINDOW_WIDTH / 2;
    data->camera_y = data->py - WINDOW_HEIGHT / 2;
}

static void world_to_screen(t_mlx_data *data, double world_x, double world_y, int *screen_x, int *screen_y)
{
    *screen_x = (int)(world_x - data->camera_x);
    *screen_y = (int)(world_y - data->camera_y);
}

static int is_on_screen(int screen_x, int screen_y)
{
    return (screen_x >= 0 && screen_x < WINDOW_WIDTH && screen_y >= 0 && screen_y < WINDOW_HEIGHT);
}

static void img_pixel_put(t_mlx_data *data, int x, int y, int color)
{
    char    *dst;
    if (is_on_screen(x, y))
    {
        dst = data->addr + (y * data->line_len + x * (data->bpp / 8));
        *(unsigned int*)dst = color;
    }
}

static void draw_square(t_mlx_data *data, int map_x, int map_y, int color)
{
    int world_x = map_y * TILE_SIZE;
    int world_y = map_x * TILE_SIZE;
    for (int i = 1; i < TILE_SIZE; i++)
    {
        for (int j = 1; j < TILE_SIZE; j++)
        {
            int screen_x, screen_y;
            world_to_screen(data, world_x + j, world_y + i, &screen_x, &screen_y);
            img_pixel_put(data, screen_x, screen_y, color);
        }
    }
}

static int is_valid_position(t_mlx_data *data, double x, double y)
{
    int map_x = (int)(y / TILE_SIZE);
    int map_y = (int)(x / TILE_SIZE);
    if (map_x < 0 || map_x >= data->rows || map_y < 0 || map_y >= data->columns)
        return 0;
    return (data->map[map_x][map_y] != '1');
}

static void draw_rays(t_mlx_data *data)
{
    double fov = data->fov;
    for (int i = 0; i < NUM_RAYS; i++)
    {
        double ray_angle = data->pa - fov / 2 + (i * fov / NUM_RAYS);
        double ray_dir_x = cos(ray_angle);
        double ray_dir_y = sin(ray_angle);
        double x = data->px;
        double y = data->py;
        double step_size = 0.5;
        while (is_valid_position(data, x, y))
        {
            int screen_x, screen_y;
            world_to_screen(data, x, y, &screen_x, &screen_y);
            if (is_on_screen(screen_x, screen_y) && 
                (int)x % TILE_SIZE != 0 && (int)y % TILE_SIZE != 0)
                img_pixel_put(data, screen_x, screen_y, 0xFF0000);
            x += ray_dir_x * step_size;
            y += ray_dir_y * step_size;
        }
    }
}

static void draw_player(t_mlx_data *data)
{
    int center_x = WINDOW_WIDTH / 2;
    int center_y = WINDOW_HEIGHT / 2;
    int start_x = center_x - PLAYER_SIZE / 2;
    int start_y = center_y - PLAYER_SIZE / 2;
    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        for (int j = 0; j < PLAYER_SIZE; j++)
            img_pixel_put(data, start_x + i, start_y + j, 0x0000FF);
    }
}

static void draw_map(t_mlx_data *data)
{
    // Clear image (set all to black)
    memset(data->addr, 0, WINDOW_HEIGHT * data->line_len);

    update_camera(data);
    int start_map_x = (int)(data->camera_y / TILE_SIZE) - 1;
    int end_map_x = (int)((data->camera_y + WINDOW_HEIGHT) / TILE_SIZE) + 1;
    int start_map_y = (int)(data->camera_x / TILE_SIZE) - 1;
    int end_map_y = (int)((data->camera_x + WINDOW_WIDTH) / TILE_SIZE) + 1;
    if (start_map_x < 0) start_map_x = 0;
    if (end_map_x >= data->rows) end_map_x = data->rows - 1;
    if (start_map_y < 0) start_map_y = 0;
    if (end_map_y >= data->columns) end_map_y = data->columns - 1;
    for (int x = start_map_x; x <= end_map_x; x++)
    {
        for (int y = start_map_y; y <= end_map_y; y++)
        {
            if (data->map[x][y] == '1')
                draw_square(data, x, y, 0x404040);
            else if (data->map[x][y] == '0')
                draw_square(data, x, y, 0xFFFFFF);
        }
    }
    draw_rays(data);
    draw_player(data);

    mlx_put_image_to_window(data->mlx, data->window, data->img, 0, 0);
}

static int handle_key(int keycode, void *param)
{
    t_mlx_data *data = (t_mlx_data *) param;
    if (keycode == XK_Escape)
        close_window();
    if (keycode == XK_w)
    {
        double new_px = data->px + cos(data->pa) * MOVE_SPEED;
        double new_py = data->py + sin(data->pa) * MOVE_SPEED;
        if (is_valid_position(data, new_px, new_py))
        {
            data->px = new_px;
            data->py = new_py;
        }
    }
    else if (keycode == XK_s)
    {
        double new_px = data->px - cos(data->pa) * MOVE_SPEED;
        double new_py = data->py - sin(data->pa) * MOVE_SPEED;
        if (is_valid_position(data, new_px, new_py))
        {
            data->px = new_px;
            data->py = new_py;
        }
    }
    else if (keycode == XK_a)
    {
        double new_px = data->px + cos(data->pa - PI/2) * MOVE_SPEED;
        double new_py = data->py + sin(data->pa - PI/2) * MOVE_SPEED;
        if (is_valid_position(data, new_px, new_py))
        {
            data->px = new_px;
            data->py = new_py;
        }
    }
    else if (keycode == XK_d)
    {
        double new_px = data->px + cos(data->pa + PI/2) * MOVE_SPEED;
        double new_py = data->py + sin(data->pa + PI/2) * MOVE_SPEED;
        if (is_valid_position(data, new_px, new_py))
        {
            data->px = new_px;
            data->py = new_py;
        }
    }
    if (keycode == XK_Left)
    {
        data->pa -= ROT_SPEED;
        if (data->pa < 0)
            data->pa += 2 * PI;
    }
    else if (keycode == XK_Right)
    {
        data->pa += ROT_SPEED;
        if (data->pa >= 2 * PI)
            data->pa -= 2 * PI;
    }
    draw_map(data);
    return (0);
}

int main()
{
    t_mlx_data data;
    data.map = copy_map(map);
    calc_rows_columns(&data);
    find_Player(&data);
    data.fov = PI / 3;
    data.mlx = mlx_init();
    if (!data.mlx) return (1);
    data.window = mlx_new_window(data.mlx, WINDOW_WIDTH, WINDOW_HEIGHT, "CUB3D - Mini Map");
    if (!data.window) return (1);
    data.img = mlx_new_image(data.mlx, WINDOW_WIDTH, WINDOW_HEIGHT);
    data.addr = mlx_get_data_addr(data.img, &data.bpp, &data.line_len, &data.endian);
    mlx_hook(data.window, 17, 0, &close_window, NULL);
    mlx_hook(data.window, 2, 1L<<0, &handle_key, &data);
    draw_map(&data);
    mlx_loop(data.mlx);
}
