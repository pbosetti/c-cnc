#include <cairo/cairo-pdf.h>
#include <cairo/cairo.h>
#include <math.h>
#include <stdlib.h>

#ifndef PLOT_H
#define PLOT_H

typedef struct polt_struct plot_t;

plot_t *plot_new(const char *file, double width, double height, double margin);

void plot_page_init(plot_t *p, double min_x, double min_y, double max_x,
               double max_y);

void plot_set_rgbaw(plot_t *p, double r, double g, double b, double a,
                    double w);

void plot_set_rgbas(plot_t *p, double r, double g, double b, double a,
                    double s);

void plot_begin_poly(plot_t *p, double x, double y);

size_t plot_segment_to(plot_t *p, double x, double y);

static size_t plot_arc_ftr(plot_t *p, double xf, double yf, double xt,
                           double yt, double r, cairo_bool_t cw);

size_t plot_arc_tr_cw(plot_t *p, double xt, double yt, double r);

size_t plot_arc_tr_ccw(plot_t *p, double xt, double yt, double r);

size_t plot_arc_tij_cw(plot_t *p, double xt, double yt, double i, double j);

size_t plot_arc_tij_ccw(plot_t *p, double xt, double yt, double i, double j);

void plot_end_poly(plot_t *p);

void plot_range(plot_t *p, double min_x, double min_y, double max_x,
                double max_y);

void plot_grid(plot_t *p, double min_x, double min_y, double max_x, double 
               max_y, double space);

void plot_text_at(plot_t *p, char const *txt, double x, double y);

void plot_close_page(plot_t *p);

void plot_close(plot_t *p);

#endif