/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 11:04:08 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/07 11:17:37 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./library/minilibx-linux/mlx.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/keysym.h>
#include <math.h>

#define TILE_SIZE 30
#define PLAYER_SIZE 10
// #define PI 3.1415926535
// #define NUM_RAYS 60

typedef struct s_mlx_data
{
	void	*mlx;
	void	*window;
    int     columns;
    int     rows;
    char    **map;
    char    player;
    double   px;
    double   py;
    double   old_px;
    double   old_py;
    // double   pdx;
    // double   pdy;
    // double   pa;
    // double  dir;
    // double  fov;
}			t_mlx_data;

// map
char *map[] = {
        "11111111111111111111111111111",
        "10000000011111000000011110001",
        "10111000000011100000000000001",
        "10111111000011100001111000001",
        "10001000000000000000001111111",
        "10000000000000000000000000001",
        "10000000000000E00000000000001",
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
            }
        }
    }
}


static int	close_window()
{
	exit(0);
}

// draw square
static void draw_square(t_mlx_data *data, int x, int y, int color)
{
    // we start from 1 because we need to skip 1 pixel because we need to display the map like a grid map
    for (int i = 1; i < TILE_SIZE; i++)
    {
        for (int j = 1; j < TILE_SIZE; j++)
            mlx_pixel_put(data->mlx, data->window, y * TILE_SIZE + j, x * TILE_SIZE + i, color);
    }
}

static void draw_player(t_mlx_data *data)
{
    int start_x = data->px * TILE_SIZE + (TILE_SIZE - PLAYER_SIZE) / 2;
    int start_y = data->py * TILE_SIZE + (TILE_SIZE - PLAYER_SIZE) / 2;

    for (int i = 0; i < PLAYER_SIZE; i++)
    {
        for (int j = 0; j < PLAYER_SIZE; j++)
            mlx_pixel_put(data->mlx, data->window, start_y + j, start_x + i, 0x0000FF);
    }
}

static void update_position_player(t_mlx_data *data)
{
    // draw the old position of player like floor
    draw_square(data, data->old_px, data->old_py, 0xFFFFFF);
    
    // draw the new position of player
    draw_player(data);

    // copy new position of player to old
    data->old_px = data->px;
    data->old_py = data->py;
}



static int handle_key(int keycode, void *param)
{
    t_mlx_data *data = (t_mlx_data *) param;

    if (keycode == XK_Escape) // ESC key
        close_window();
    
    // key up arrow
    if (keycode == XK_w)
    {
        if (data->map[(int)data->px - 1][(int)data->py] == '1')
            return (1);
        data->px -= 1;
        update_position_player(data);
    }
    // key down arrow
    else if (keycode == XK_s)
    {
       if (data->map[(int)data->px + 1][(int)data->py] == '1')
            return (1);
        data->px += 1;
        update_position_player(data);
    }
    // key left arrow
    else if (keycode == XK_a)
    {
        if (data->map[(int)data->px][(int)data->py - 1] == '1')
            return (1);
        data->py -= 1;
        update_position_player(data);
    }
    // key right arrow
    else if (keycode == XK_d)
    {
       if (data->map[(int)data->px][(int)data->py + 1] == '1')
            return (1);
        data->py += 1;
        update_position_player(data);
    }
    return (0);
}


// draw map
static void draw_map(t_mlx_data *data)
{
    for (int x = 0; data->map[x]; x++)
    {
        for (int y = 0; data->map[x][y]; y++)
        {
            if (data->map[x][y] == '1')
                draw_square(data, x, y, 0xFF0000);
            else if (data->map[x][y] == '0')
                draw_square(data, x, y, 0xFFFFFF);
            else
                break ;
        }
    }
    // draw player
    draw_player(data);
}

int main()
{
    t_mlx_data data;

    // copy the map
    data.map = copy_map(map);

    // add columns and rows
    calc_rows_columns(&data);

    // find player
    find_Player(&data);
    
    // init Field of View of player
    // data.fov = PI / 3 ; // 60 degrees

    data.mlx = mlx_init();
    if (!data.mlx)
        return (1);

    data.window = mlx_new_window(data.mlx, data.columns * TILE_SIZE, data.rows * TILE_SIZE, "CUB3D");
    if (!data.window)
        return (1);
    mlx_hook(data.window, 17, 0, &close_window, NULL);
        
    // move the player
    mlx_hook(data.window, 2, 1L<<0, &handle_key, &data);

    // draw the map
    draw_map(&data);

    // loop the window to still display
    mlx_loop(data.mlx);
}
