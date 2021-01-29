#include "plot.h"

typedef struct polt_struct {
  cairo_surface_t *surface;
  cairo_t *cr;
  double width, height, margin, scale;
  size_t count;
} plot_t;

typedef struct {
  double x, y;
} point_t;

plot_t *plot_new(const char *file, double width, double height, double margin) {
  plot_t *p = malloc(sizeof(plot_t));
  p->height = height;
  p->width = width;
  p->margin = margin;
  p->surface = cairo_pdf_surface_create(file, width, height);
  p->cr = cairo_create(p->surface);
  p->count = 0;
  p->scale = 1;
  return p;
}

void plot_page_init(plot_t *p, double min_x, double min_y, double max_x, double max_y) {
  cairo_matrix_t x_reflection_matrix, font_reflection_matrix;
  point_t rng = {
    max_x - min_x,
    max_y - min_y
  };
  point_t wh = {
    p->width-p->margin,
    p->height-p->margin
  };
  double ratio = wh.x/wh.y;
  double scale;
  if ((ratio > 1 && rng.x/rng.y >1) || (ratio < 1 && rng.x/rng.y < 1))
    scale  = fmax(wh.x, wh.y) / fmax(rng.x, rng.y);
  else
    scale  = fmin(wh.x, wh.y) / fmax(rng.x, rng.y);
  p->scale = scale;

  rng.x = (wh.x) / scale;
  rng.y = (wh.y) / scale;

  cairo_identity_matrix(p->cr);

  // margin
  cairo_set_source_rgba(p->cr, 0, 0, 0, 0.3);
  cairo_set_line_width(p->cr, 2);
  cairo_rectangle(p->cr, p->margin/2.0, p->margin/2.0, wh.x, wh.y);
  cairo_clip(p->cr);
  cairo_new_path(p->cr);
  cairo_rectangle(p->cr, p->margin/2.0, p->margin/2.0, wh.x, wh.y);
  cairo_stroke(p->cr);

  // change coordinates from Gamma to L
  cairo_matrix_init_identity(&x_reflection_matrix); 
  x_reflection_matrix.yy = -1.0;
  cairo_set_matrix(p->cr, &x_reflection_matrix);
  cairo_translate(p->cr, p->margin/2, -p->height+p->margin/2);

  // resize for holding'em all
  cairo_scale(p->cr, scale, scale);
  cairo_translate(p->cr, -min_x, -min_y);

  // axes
  cairo_set_source_rgb(p->cr, 0, 0, 0);
  cairo_set_line_width(p->cr, 1/scale);
  cairo_move_to(p->cr, min_x, 0);
  cairo_line_to(p->cr, rng.x + min_x, 0);
  cairo_move_to(p->cr, 0, min_y);
  cairo_line_to(p->cr, 0, rng.y + min_y);
  cairo_stroke(p->cr);

  // font reflection
  cairo_matrix_init_identity(&font_reflection_matrix);
  cairo_set_font_matrix(p->cr, &font_reflection_matrix);
  cairo_set_font_size(p->cr, 10/scale);
  cairo_get_font_matrix(p->cr, &font_reflection_matrix);
  font_reflection_matrix.yy *= -1;
  cairo_set_font_matrix(p->cr, &font_reflection_matrix);

  cairo_set_line_join(p->cr, CAIRO_LINE_JOIN_ROUND);
  cairo_set_line_cap(p->cr, CAIRO_LINE_CAP_ROUND);
}

void plot_set_rgbaw(plot_t *p, double r, double g, double b, double a, double w) {
  cairo_set_source_rgba(p->cr, r, g, b, a);
  cairo_set_line_width(p->cr, w/p->scale);
}

void plot_set_rgbas(plot_t *p, double r, double g, double b, double a, double w) {
  cairo_set_source_rgba(p->cr, r, g, b, a);
  cairo_set_line_width(p->cr, w);
}

void plot_begin_poly(plot_t *p, double x, double y) {
  double lw = cairo_get_line_width(p->cr);
  cairo_arc(p->cr, x, y, 2*lw, 0, 2*M_PI);
  cairo_fill(p->cr);
  cairo_stroke(p->cr);
  cairo_move_to(p->cr, x, y);
  p->count = 0;
}

size_t plot_segment_to(plot_t *p, double x, double y) {
  cairo_line_to(p->cr, x, y);
  return p->count++;
}

static size_t plot_arc_ftr(plot_t *p, double xf, double yf, double xt, double yt, double r, cairo_bool_t cw) {
  double d = sqrt(pow(xf-xt, 2) + pow(yf-yt, 2));
  double l = d/2.0;
  double h = sqrt(pow(r,2)-pow(l,2));
  int s = r/fabs(r) * (cw ? 1 : -1);
  double x = l/d*(xt-xf)+s*h/d*(yt-yf)+xf;
  double y = l/d*(yt-yf)-s*h/d*(xt-xf)+yf;
  double af = atan2(yf-y, xf-x);
  double at = atan2(yt-y, xt-x);
  if (cw)
    cairo_arc_negative(p->cr, x, y, fabs(r), af, at);
  else
    cairo_arc(p->cr, x, y, fabs(r), af, at);
  return p->count++;
}

size_t plot_arc_tr_cw(plot_t *p, double xt, double yt, double r) {
  double xf, yf;
  cairo_get_current_point(p->cr, &xf, &yf);
  return plot_arc_ftr(p, xf, yf, xt, yt, r, 1);
}

size_t plot_arc_tr_ccw(plot_t *p, double xt, double yt, double r) {
  double xf, yf;
  cairo_get_current_point(p->cr, &xf, &yf);
  return plot_arc_ftr(p, xf, yf, xt, yt, r, 0);
}

size_t plot_arc_tij_cw(plot_t *p, double xt, double yt, double i, double j) {
  double xf, yf;
  double r = sqrt(pow(i, 2) + pow(j, 2));
  cairo_get_current_point(p->cr, &xf, &yf);
  return plot_arc_ftr(p, xf, yf, xt, yt, r, 1);
}

size_t plot_arc_tij_ccw(plot_t *p, double xt, double yt, double i, double j) {
  double xf, yf;
  double r = sqrt(pow(i, 2) + pow(j, 2));
  cairo_get_current_point(p->cr, &xf, &yf);
  return plot_arc_ftr(p, xf, yf, xt, yt, r, 0);
}

void plot_end_poly(plot_t *p) {
  cairo_stroke(p->cr);
}

void plot_range(plot_t *p, double min_x, double min_y, double max_x, double max_y) {
  cairo_rectangle(p->cr, min_x, min_y, max_x - min_x, max_y - min_y);
  cairo_fill(p->cr);
}

void plot_grid(plot_t *p, double min_x, double min_y, double max_x, double max_y, double space) {
  int i, j;
  double l = space / 10;
  int is = (int)(min_x/space);
  int ie = (int)(max_x/space);
  int js = (int)(min_y/space);
  int je = (int)(max_y/space);
  for (i = is; i <= ie; i++) {
    for (j = js; j <= je; j++) {
      cairo_move_to(p->cr, space*i - l/2, space*j);
      cairo_line_to(p->cr, space*i + l/2, space*j);
      cairo_stroke(p->cr);
      cairo_move_to(p->cr, space*i, space*j - l/2);
      cairo_line_to(p->cr, space*i, space*j + l/2);
      cairo_stroke(p->cr);
    }
  }
}

void plot_text_at(plot_t *p, char const *txt, double x, double y) {
  cairo_move_to(p->cr, x, y);
  cairo_show_text(p->cr, txt);
}

void plot_close_page(plot_t *p) {
  cairo_show_page(p->cr);
}

void plot_close(plot_t *p) {
  cairo_destroy(p->cr);
  cairo_surface_destroy(p->surface);
  free(p);
}