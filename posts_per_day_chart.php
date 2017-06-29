<?php
/**
 * Posts per day chart
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params:
 * @param int    "width"
 * @param int    "height"
 * @param int    "id_author"
 * @param int    "id_category"
 */

use hng2_base\accounts_repository;
use hng2_modules\categories\categories_repository;

include "../config.php";
include "../includes/bootstrap.inc";
include "../lib/phplot-6.1.0/rgb.inc.php";
include "../lib/phplot-6.1.0/phplot.php";

if( $account->state != "enabled" ) throw_fake_401();
if( empty($_GET["id_author"])   ) die("Missing author");

$accounts_repository = new accounts_repository();
$author = $accounts_repository->get($_GET["id_author"]);
if( is_null($author) ) die("Author doesn't exist");

if( ! empty($_GET["id_category"]) )
{
    $categories_repository = new categories_repository();
    $category = $categories_repository->get($_GET["id_category"]);
    if( is_null($category) ) die("category Doesn't exist");
}

$days  = 90;
$date  = date("Y-m-d 00:00:00", strtotime("today - $days days"));
$data  = array();
$cat   = empty($_GET["id_category"]) ? "" : "and main_category = '{$_GET["id_category"]}'";
$query = "select date(publishing_date) as publishing_date, count(id_post) as total 
          from posts
          where id_author = '{$_GET["id_author"]}'
          and   date(publishing_date) >= '$date'
          $cat
          group by date(publishing_date)";
$res    = $database->query($query);

while($row = $database->fetch_object($res)) $data[$row->publishing_date] = $row->total;

$current_date =
$first_date   = key($data);
$today        = date("Y-m-d");
$final_data   = array();
$prev_month   = "";
while( $current_date <= $today )
{
    $title = $current_date == $first_date || $current_date == $today || $data[$current_date] > 0
           ? date("Md", strtotime($current_date))
           : "";
    
    if( ! empty($title) )
    {
        if( $prev_month != date("M", strtotime($current_date)) )
            $title = date("Md", strtotime($current_date));
        else
            $title = date("d", strtotime($current_date));
        $prev_month = date("M", strtotime($current_date));
    }
    
    $final_data[] = array($title, $current_date, $data[$current_date]);
    $current_date = date("Y-m-d", strtotime("$current_date + 1 day"));
}
$data = $final_data;

$width  = empty($_REQUEST["width"])  ? 620 : $_REQUEST["width"];
$height = empty($_REQUEST["height"]) ? 380 : $_REQUEST["height"];

$plot = new PHPlot($width, $height);
# $plot->SetImageBorderType('plain');

$plot->SetPlotType('bars');
$plot->SetShading('none');
$plot->SetDataType('text-data');
$plot->SetNumberFormat(".", ";");
$plot->SetDataValues($data);
$plot->SetLineWidths(2);
$plot->SetDataColors("SkyBlue");

# Turn on Y data labels:
$plot->SetYDataLabelPos('plotin');

# With Y data labels, we don't need Y ticks or their labels, so turn them off.
$plot->SetYTickLabelPos('none');
$plot->SetYTickPos('none');

# Main plot title:
$title = empty($_GET["id_category"])
    ? replace_escaped_objects(
          $current_module->language->user_profile_sections->charts->all,
          array('{$user}' => $author->display_name)
      )
    : replace_escaped_objects(
          $current_module->language->user_profile_sections->charts->by_category,
          array('{$user}' => $author->display_name, '{$category}' => $category->title)
      );

$title .= " ($days {$language->time->days})";
$plot->SetTitle(utf8_decode($title));

# Set Y data limits, tick increment, and titles:
# $plot->SetPlotAreaWorld(NULL, 0, NULL, NULL);
# $plot->SetYTickIncrement(10);
# $plot->SetYTitle(trim($current_module->language->widgets->registrations_chart->y_title));
# $plot->SetXTitle(trim($current_module->language->widgets->registrations_chart->x_title));

# Colors are significant to this data:
# $plot->SetDataColors(array('red', 'green', 'blue', 'yellow', 'cyan', 'magenta'));
$plot->SetMarginsPixels(null, null, null, null);
# $plot->SetLegendPixels(($width - 80), 25);
# $plot->SetLegend($leyendas);
# $plot->SetLegendUseShapes(true);

$plot->SetDrawXDataLabelLines(True);
$plot->SetDrawXGrid(false);
$plot->SetDrawYGrid(false);

# Turn off X tick labels and ticks because they don't apply here:
$plot->SetXTickLabelPos('none');
$plot->SetXTickPos('none');

$plot->DrawGraph();
