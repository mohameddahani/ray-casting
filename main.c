/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 11:04:08 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/04 16:19:53 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./library/minilibx-linux/mlx.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/keysym.h>

#define TILE_SIZE 40

typedef struct s_mlx_data
{
	void	*mlx;
	void	*window;
    int     columns;
    int     rows;
    char    **map;
}			t_mlx_data;

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

// calc the rows and columns
static void calc_rows_columns(t_mlx_data *data)
{
    data->rows = 0;
    data->columns = 0;
    while (data->map[data->rows])
        data->rows++;    
    data->columns = strlen(data->map[0]);
}

static int	close_window()
{
	exit(0);
}


static int handle_key(int keycode)
{
    if (keycode == XK_Escape) // ESC key
        close_window();
    
    // key up arrow
    if (keycode == XK_Up)
    {
        printf("UP\n");
    }
    // key down arrow
    else if (keycode == XK_Down)
    {
        printf("DOWN\n");
    }
    // key left arrow
    else if (keycode == XK_Left)
    {
        printf("LEFT\n");
    }
    // key right arrow
    else if (keycode == XK_Right)
    {
        printf("RIGHT\n");
    }
    return (0);
}


// draw square
static void draw_square(t_mlx_data *data, int x, int y, int color)
{
    for (int i = 0; i < TILE_SIZE; i++)
    {
        for (int j = 0; j < TILE_SIZE; j++)
            mlx_pixel_put(data->mlx, data->window, y * TILE_SIZE + j, x * TILE_SIZE + i, color);
    }
}

// draw map
static int draw_map(void *param)
{
    t_mlx_data *data = (t_mlx_data *) param;

    for (int x = 0; data->map[x]; x++)
    {
        for (int y = 0; data->map[x][y]; y++)
        {
            if (data->map[x][y] == '1')
                draw_square(data, x, y, 0xFF0000);
            else if (data->map[x][y] == '0')
                draw_square(data, x, y, 0xFFFFFF);
            else if (data->map[x][y] == 'N')
                draw_square(data, x, y, 0x0000FF);
            else
                break ;
        }
    }
    return (0);
}

int main()
{
    t_mlx_data data;

    // copy the map
    data.map = map;

    // add columns and rows
    calc_rows_columns(&data);
    
    data.mlx = mlx_init();
    if (!data.mlx)
        return (1);

    data.window = mlx_new_window(data.mlx, data.columns * TILE_SIZE, data.rows * TILE_SIZE, "CUB3D");
    if (!data.window)
        return (1);
    mlx_hook(data.window, 17, 0, &close_window, NULL);
        
    // move the player
    mlx_hook(data.window, 2, 1L<<0, &handle_key, NULL);

    // draw the map
    mlx_loop_hook(data.mlx, &draw_map, &data);

    // loop the window to still display
    mlx_loop(data.mlx);
}
