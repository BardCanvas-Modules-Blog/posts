<?php
/**
 * Posts index buttons
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

include "../config.php";
include "../includes/bootstrap.inc";
if( ! $account->_is_admin ) throw_fake_404();
session_start();
