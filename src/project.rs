/* ===============================================================================
Моделирование эволюции живого мира.
Настройки модели и экземпляра программы.
27 Jan 2021.
----------------------------------------------------------------------------
Licensed under the terms of the GPL version 3.
http://www.gnu.org/licenses/gpl-3.0.html
Copyright (c) 2013-2021 by Artem Khomenko _mag12@yahoo.com.
=============================================================================== */

/*pub struct Globals {
   world_size: WorldSize,
}

pub struct WorldSize {
   with: usize,
   height_ratio: f64,
}*/

/* pub struct Locals {
   // appearance: WindowState,
   top_menu: TopMenuState,
} */

/*pub enum WindowState {
   Windowed(Point, Point), // Position (left, top) and size (with, height)
   Maximized,
   Minimized,
}

pub struct Point {
   x: isize,
   y: isize,
}*/

/* pub struct TopMenuState {
   visible: bool,
   illuminate: bool,
} */

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Preset {
    Custom,
    XKCD,
    /* Glider,
    SmallExploder,
    Exploder,
    TenCellRow,
    LightweightSpaceship,
    Tumbler,
    GliderGun,
    Acorn, */
}

/* pub static ALL: &[Preset] = &[
    Preset::Custom,
    Preset::XKCD,
    Preset::Glider,
    Preset::SmallExploder,
    Preset::Exploder,
    Preset::TenCellRow,
    Preset::LightweightSpaceship,
    Preset::Tumbler,
    Preset::GliderGun,
    Preset::Acorn,
]; */

impl Preset {
    pub fn life(self) -> Vec<(isize, isize)> {
        #[rustfmt::skip]
        let cells = match self {
            Preset::Custom => vec![],
            Preset::XKCD => vec![
                "  xxx  ",
                "  x x  ",
                "  x x  ",
                "   x   ",
                "x xxx  ",
                " x x x ",
                "   x  x",
                "  x x  ",
                "  x x  ",
            ],
            /* Preset::Glider => vec![
                " x ",
                "  x",
                "xxx"
            ],
            Preset::SmallExploder => vec![
                " x ",
                "xxx",
                "x x",
                " x ",
            ],
            Preset::Exploder => vec![
                "x x x",
                "x   x",
                "x   x",
                "x   x",
                "x x x",
            ],
            Preset::TenCellRow => vec![
                "xxxxxxxxxx",
            ],
            Preset::LightweightSpaceship => vec![
                " xxxxx",
                "x    x",
                "     x",
                "x   x ",
            ],
            Preset::Tumbler => vec![
                " xx xx ",
                " xx xx ",
                "  x x  ",
                "x x x x",
                "x x x x",
                "xx   xx",
            ],
            Preset::GliderGun => vec![
                "                        x           ",
                "                      x x           ",
                "            xx      xx            xx",
                "           x   x    xx            xx",
                "xx        x     x   xx              ",
                "xx        x   x xx    x x           ",
                "          x     x       x           ",
                "           x   x                    ",
                "            xx                      ",
            ],
            Preset::Acorn => vec![
                " x     ",
                "   x   ",
                "xx  xxx",
            ], */
        };

        let start_row = -(cells.len() as isize / 2);

        cells
            .into_iter()
            .enumerate()
            .flat_map(|(i, cells)| {
                let start_column = -(cells.len() as isize / 2);

                cells
                    .chars()
                    .enumerate()
                    .filter(|(_, c)| !c.is_whitespace())
                    .map(move |(j, _)| {
                        (start_row + i as isize, start_column + j as isize)
                    })
            })
            .collect()
    }
}

impl Default for Preset {
    fn default() -> Preset {
        Preset::XKCD
    }
}

impl std::fmt::Display for Preset {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "{}",
            match self {
                Preset::Custom => "Custom",
                Preset::XKCD => "xkcd #2293",
                /* Preset::Glider => "Glider",
                Preset::SmallExploder => "Small Exploder",
                Preset::Exploder => "Exploder",
                Preset::TenCellRow => "10 Cell Row",
                Preset::LightweightSpaceship => "Lightweight spaceship",
                Preset::Tumbler => "Tumbler",
                Preset::GliderGun => "Gosper Glider Gun",
                Preset::Acorn => "Acorn", */
            }
        )
    }
}
