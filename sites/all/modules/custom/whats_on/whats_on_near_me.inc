<?php
/**
 * @file
 * whats_on_near_me.inc
 */

/**
 * implements whats_on_near_me function
 */
function whats_on_near_me(){
    $output = array();

    $cinemas = whats_on_cinema_json();


    $output[] = array(
      '#type' => 'markup',
      '#markup' => theme(
        'cinema_near_me' , array(
              'cinema' => $cinemas
          )
    ),
    );
    $output[]['#attached']['js']['https://maps.googleapis.com/maps/api/js'] = array('preprocess' => TRUE);

    $output[]['#attached']['js'][drupal_get_path('module', 'whats_on') . '/assets/custom.js'] = array('preprocess' => TRUE);
    return $output;


}


/**
 * Implements hook_theme().
 */
function whats_on_theme() {
    return array(
        'cinema_near_me' => array(
            'template' => 'cinema_near_me',
            'variables' => array(
                'cinema' => NULL,
            ),
            'path' => drupal_get_path('module' , 'whats_on') . '/template',
        ),
    );
}
/**
 * Implements whats_on_cinema_json().
 */
function whats_on_cinema_json(){
    module_load_include('inc', 'whats_on', 'whats_on_dates');
    $cinemas = whats_on_cinema_id();
    $marker = array();
    foreach ($cinemas as $cine) {
        $node = node_load($cine);
        $address = $node->field_cinema_adress[LANGUAGE_NONE][0]['value'];
        $encode = urlencode($address);
        $lat_long = drupal_http_request('http://maps.google.com/maps/api/geocode/json?address='.$encode.'&sensor=false');

        if($lat_long->code == 200 ) {
            $lat_long_data = json_decode($lat_long->data);
            dsm($lat_long_data);
//            $lat = $lat_long->results[0]->geometry->location->lat;
//            $lng = $lat_long->results[0]->geometry->location->lng;
         //   $marker[] = array($node->title , $lat , $lng);
        }
    }
    dsm($marker);

}
/**
 * Implements whats_on_lat_lng().
 *
 */
function whats_on_lat_lng($address){

}