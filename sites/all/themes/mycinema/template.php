<?php

/**
 * @file
 * template.php
 */
function mycinema_preprocess_block(&$vars) {
   //   dpm($vars);
}
function mycinema_preprocess_page(&$vars, $hook)
{

    if (true) {
       // drupal_add_js(drupal_get_path('theme', 'mycinema') . '/js/custom.js');

        $vars['scripts'] = drupal_get_js(); // necessary in D7?
    }

    //add news class
    //dpm( $vars['navbar_classes_array']);
    //$vars['navbar_classes_array'][] = 'new-class';
    //remove class
    //unset($vars['navbar_classes_array'][1]);
    // you need change value 0 to value you want remove class
    // Default
    // 0  'navbar', 1 => 'container', 2 => 'navbar-default'

}