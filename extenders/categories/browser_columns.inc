<?php
/**
 * Categories extender - browser data columns
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Globals:
 * @var module $current_module accounts
 * @var module $this_module    self
 * @var object $row
 */

use hng2_base\module;

global $row;

echo "<td nowrap align=\"right\">"
.    ( empty($row->_posts_count) ? "&mdash;" : number_format($row->_posts_count) )
.    "</td>";
