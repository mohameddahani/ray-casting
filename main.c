/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 11:04:08 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/04 11:36:30 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./library/minilibx-linux/mlx.h"
#include <unistd.h>
#include <stdlib.h>

typedef struct s_mlx_data
{
	void	*mlx;
	void	*window;
}			t_mlx_data;

static int	close_window()
{
	exit(0);
}


static int handle_key(int keycode)
{
    if (keycode == 65307) // ESC key
        close_window();
    return (0);
}

int main()
{
    t_mlx_data data;
    
    data.mlx = mlx_init();
    if (!data.mlx)
        return (1);
    data.window = mlx_new_window(data.mlx, 1000, 700, "CUB3D");
    if (!data.window)
        return (1);
    mlx_hook(data.window, 17, 0, &close_window, NULL);
    mlx_hook(data.window, 2, 1L<<0, &handle_key, NULL);

    // loop the window to still display
    mlx_loop(data.mlx);
}