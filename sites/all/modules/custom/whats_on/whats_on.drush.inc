<?php
/**
 * @file
 * whats_on.drush.inc
 */

/**
 * Implements drush_command hook
 */
function whats_on_drush_command() {
    $items = array();
    $items['whats-on-dates'] = array(
        'callback' => 'whats_on_drush_import_times',
        'description' => "Bulk imports dates and times from cineworld api",
        'examples' => array(
            'drush wo-d' => 'Bulk imports dates',
        ),
        'aliases' => array('wo-d'),
    );
    $items['whats-on-save-cinemas'] = array(
        'callback' => 'whats_on_drush_import_cinemas',
        'description' => "Bulk imports dates and times from cineworld api",
        'examples' => array(
            'drush wo-cine' => 'Bulk imports cinemas',
        ),
        'aliases' => array('wo-cine'),
    );
    $items['whats-on-save-films'] = array(
        'callback' => 'whats_on_api_integration',
        'description' => "Bulk imports new films via OMBD",
        'examples' => array(
            'drush wo-films' => 'Bulk imports films',
        ),
        'aliases' => array('wo-films'),
    );

    return $items;
}
/**
 * Implemtnts the whats on cineworld function
 */
function whats_on_drush_import_times(){
    module_load_include('inc', 'whats_on', 'whats_on_dates');
    whats_on_cineworld();
}
/**
 * Implemtnts the whats on whats_on_cinema_integration
 */
function whats_on_drush_import_cinemas(){
    module_load_include('inc', 'whats_on', 'whats_on_cinema');
    whats_on_cinema_integration();
}



