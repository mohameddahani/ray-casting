/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 11:04:08 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/08 16:03:40 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./library/minilibx-linux/mlx.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/keysym.h>
#include <math.h>

#define TILE_SIZE 32
#define PLAYER_SIZE 10
#define PI 3.1415926535
#define NUM_RAYS 60
#define ROT_SPEED 0.1


typedef struct s_mlx_data
{
    void    *mlx;
    void    *window;
    int     columns;
    int     rows;
    char    **map;
    char    player;
    double  px;
    double  py;
    double  old_px;
    double  old_py;
    double  pa;
    double  fov;
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

// calc the rows and columns
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
                data->px = x;
                data->py = y;
                data->old_px = x;
                data->old_py = y;
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

// draw square
static void draw_square(t_mlx_data *data, int x, int y, int color)
{
    for (int i = 1; i < TILE_SIZE; i++)
    {
        for (int j = 1; j < TILE_SIZE; j++)
            mlx_pixel_put(data->mlx, data->window, y * TILE_SIZE + j, x * TILE_SIZE + i, color);
    }
}

// draw rays using step-based algorithm
static void draw_rays(t_mlx_data *data)
{
    double fov = data->fov;

    for (int i = 0; i < NUM_RAYS; i++)
    {
        double ray_angle = data->pa - fov / 2 + (i * fov / NUM_RAYS);
        double ray_dir_x = cos(ray_angle);
        double ray_dir_y = sin(ray_angle);

        double x = data->py * TILE_SIZE + TILE_SIZE / 2;
        double y = data->px * TILE_SIZE + TILE_SIZE / 2;

        double step_size = 1.0;

        while (1)
        {
            x += ray_dir_x * step_size;
            y += ray_dir_y * step_size;

            int map_x = (int)(y / TILE_SIZE);
            int map_y = (int)(x / TILE_SIZE);

            if (map_x < 0 || map_x >= data->rows || map_y < 0 || map_y >= data->columns)
                break;

            if (data->map[map_x][map_y] == '1')
                break;

            int pixel_x = (int)x;
            int pixel_y = (int)y;
            if (pixel_x % TILE_SIZE != 0 && pixel_y % TILE_SIZE != 0)
                mlx_pixel_put(data->mlx, data->window, pixel_x, pixel_y, 0xFF0000);
        }
    }
}

static void draw_player(t_mlx_data *data)
{
    int start_x = data->py * TILE_SIZE + (TILE_SIZE - PLAYER_SIZE) / 2;
    int start_y = data->px * TILE_SIZE + (TILE_SIZE - PLAYER_SIZE) / 2;

    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        for (int j = 0; j < PLAYER_SIZE; j++)
            mlx_pixel_put(data->mlx, data->window, start_x + i, start_y + j, 0x0000FF);
    }
}

// draw map
static void draw_map(t_mlx_data *data)
{
    for (int x = 0; data->map[x]; x++)
    {
        for (int y = 0; data->map[x][y]; y++)
        {
            if (data->map[x][y] == '1')
                draw_square(data, x, y, 0x000000);
            else if (data->map[x][y] == '0')
                draw_square(data, x, y, 0xFFFFFF);
        }
    }
    draw_player(data);
    draw_rays(data);
}

static void update_position_player(t_mlx_data *data)
{
    draw_map(data);
    draw_player(data);
    draw_rays(data);
    data->old_px = data->px;
    data->old_py = data->py;
}

static int handle_key(int keycode, void *param)
{
    t_mlx_data *data = (t_mlx_data *) param;

    if (keycode == XK_Escape)
        close_window();
    
    if (keycode == XK_w && data->map[(int)data->px - 1][(int)data->py] != '1')
        data->px -= 1;
    else if (keycode == XK_s && data->map[(int)data->px + 1][(int)data->py] != '1')
        data->px += 1;
    else if (keycode == XK_a && data->map[(int)data->px][(int)data->py - 1] != '1')
        data->py -= 1;
    else if (keycode == XK_d && data->map[(int)data->px][(int)data->py + 1] != '1')
        data->py += 1;

    if (keycode == XK_Left)
    {
        data->pa -= ROT_SPEED;
        if (data->pa < 0)
            data->pa += 2 * PI;
    }
    else if (keycode == XK_Right)
    {
        data->pa += ROT_SPEED;
        if (data->pa > 2 * PI)
            data->pa -= 2 * PI;
    }
    
    update_position_player(data);
    return (0);
}



int main()
{
    t_mlx_data data;

    data.map = copy_map(map);
    calc_rows_columns(&data);
    find_Player(&data);
    data.fov = PI / 3; // 60 degrees

    data.mlx = mlx_init();
    if (!data.mlx)
        return (1);
    data.window = mlx_new_window(data.mlx, data.columns * TILE_SIZE, data.rows * TILE_SIZE, "CUB3D");
    if (!data.window)
        return (1);

    mlx_hook(data.window, 17, 0, &close_window, NULL);
    mlx_hook(data.window, 2, 1L<<0, &handle_key, &data);

    draw_map(&data);

    mlx_loop(data.mlx);
}
