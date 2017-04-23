<?php
/**
 * Accounts extender - principal data addons
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Globals:
 * @var module $current_module accounts
 * @var module $this_module    self
 */

use hng2_base\module;

global $row, $principal_data_addons, $modules;

if( empty($row->_last_post_data) ) return;

list($id, $title, $date, $status) = explode("\t", $row->_last_post_data);

$chart_url = "{$this_module->get_url()}/posts_per_day_chart.php"
           . "?id_author={$row->id_account}&width=720&height=480&wasuuup=" . md5(mt_rand(1, 65535));

$principal_data_addons[] = replace_escaped_objects(
    $this_module->language->accounts_nav_columns->principal_data,
    array(
        '{$url}'                      => "{$config->full_root_path}/{$id}",
        '{$title}'                    => convert_emojis($title),
        '{$time}'                     => $date == "0000-00-00 00:00:00" ? "&mdash;" : time_mini_string($date),
        '{$status}'                   => $this_module->language->status_labels->{$status},
        '{$posts_timeline_chart_url}' => $chart_url,
    )
);
