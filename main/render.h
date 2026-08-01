#pragma once
// Rendering for each game state, rewritten on top of the AKA high-level
// gb_graphics API (drawImage) instead of the META HD slice/DMA renderer.
void render_home();
void render_run();
void render_pause();
void render_gameover();
