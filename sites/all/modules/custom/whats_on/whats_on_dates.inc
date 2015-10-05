<?php
/**
 * @file
 * whats_on_dates.inc
 */
/**
 * @return implements a whats_on_movie_code
 *  returns cineworlds film id
 */
function whats_on_movie_code() {
    // Get all node ids in the content type of film
    $movie_ids =  db_select('node' , 'n')->fields('n', array('nid'))->condition('type' , 'films' , '=')->condition('status' , 1 , '=')->execute()->fetchAll();
    $film_id = array();
    // Loop through to gett the nid
    foreach ($movie_ids as $key => $nid) {
        // Nid to movie id
        $cineworld_mid = whats_on_nid_to_mid($nid->nid);
        $release_date = $cineworld_mid['release_date'];
        $release_date = date("Y-m-d", strtotime($release_date));
        $date = date("Y-m-d" , strtotime("-2 weeks"));
        $future = date("Y-m-d" , strtotime("+1 month"));
        if ($cineworld_mid && $date < $release_date) {
            if ($release_date < $future) {
                $film_id[] = $nid->nid;
            }
        }
    }

    return $film_id;
}

/**
 * @return implements a whats_on_cineworld
 *  this is our main function which adds showing times and dates to cinemas
 */
function whats_on_cineworld() {
    // Get dates
    $date = whats_on_cineworld_dates();
    // Get all cinemas
    $cinema = whats_on_cinema_id();
    // Get our film id
    $film = whats_on_movie_code();
    $times = whats_on_cineworld_times($cinema , $date , $film);

    $time = array();
    foreach ($times as $cinema => $film) {
        foreach ($film as $name => $dates) {
            foreach ($dates as $date) {
                // cinema nid = cineworld cinema id
                $cid = whats_on_nid_to_cid($cinema);
                // movie nid = cineworld film id
                $mid = whats_on_nid_to_mid($name);
                // Get request through api
                $movie_times = drupal_http_request("http://www2.cineworld.co.uk/api/quickbook/performances?key=YEyQfFvY&cinema=" . $cid . "&film=" . $mid['film_id'] . "&date=" . $date);
                // if responce
                if ($movie_times->code == 200):
                    // data to json
                    $data = drupal_json_decode($movie_times->data);
                    // if performances
                    if (!empty($data['performances'])) {
                        foreach ($data as $key => $d) {
                            foreach ($d as $k => $times) {
                                if (!empty($times['time'])) :
                                    // add the times into an array
                                    $time[] = array('time' => $times['time'] ,  'id' => $date . '-' . $cinema . '-' . $name  );

                                endif;
                            }
                        }
                    }
                endif;
            }
        }
    }


    $showings = array();
    foreach($time as $key => $item) {
        $showings[$item['id']][$key] = $item;
    }

    // group by date
    ksort($showings, SORT_NUMERIC);
    //
    whats_on_cine_film_nid($showings);
    return;
}

/*
 * whats_on_cine_film_nid Returns an array of times to be added to entitys
 *
 */
function whats_on_cine_film_nid($data){
    $times = array();
    foreach ($data as $key => $dates) {
        $id = explode('-',$key);
        $times[] =  array(
            'date' => $id[0],
            'cinema' => $id[1],
            'film' => $id[2],
            'times' => $dates
        );
    }
    whats_on_update_entity($times);

}
/*
 * whats_on_update_entity Returns an array of times added to cinemas
 *
 */
function whats_on_update_entity($performances){

    if ($performances) {
        foreach ($performances as $cinema => $performance) {
            $date = $performance['date'];
            $film = $performance['film'];
            $my_date = date('Y-m-d', strtotime($date));
            $cinema = $performance['cinema'];
            $node = node_load($cinema);
            $film_node = node_load($film);
            if ($film_node) {
                $field_collection_value = entity_create('field_collection_item', array('field_name' => 'field_cinema_showing_times'));
                $field_collection_value->setHostEntity('node', $node);
                $field_collection_value->field_cinema_time_film[LANGUAGE_NONE][0]['value'] = $film;
                $field_collection_value->field_cinema_time_film[LANGUAGE_NONE][0]['target_id'] = $film;
                $field_collection_value->field_cinema_showing_date[LANGUAGE_NONE][0]['value'] = $my_date;
                $field_collection_value->save();
                // add to time field collection within collection
                foreach ($performance['times'] as $key => $t) {
                    $my_time = date(' - H:i:s', strtotime($t['time'] . '-1 hour'));
                    $layer_field_collection_item = entity_create('field_collection_item', array(
                        'field_name' => 'field_cinema_time',
                        'field_cinema_showing_time' => array(LANGUAGE_NONE => array(array('value' => $my_date . $my_time))),
                    ));
                    $layer_field_collection_item->setHostEntity('field_collection_item', $field_collection_value);
                    $layer_field_collection_item->save();
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}

/**
 * @return array implements whats_on_cineworld_dates
 * Gets all dates available for a cinema
 */
function whats_on_cineworld_dates() {
    $film_codes = whats_on_movie_code();
    foreach ($film_codes as $film) {
        $mid = whats_on_nid_to_mid($film);
        $movies = drupal_http_request("http://www2.cineworld.co.uk/api/quickbook/dates?key=YEyQfFvY&film=".$mid['film_id']);
        if ($movies->code == 200):
            // Decode our response
            $data = drupal_json_decode($movies->data);
            $dates = array();
            foreach ($data as $key => $date) {
                foreach ($date as $k => $d) {
                    $dates[] = $d;
                }
            }
            // If this is a movie return true;
            return $dates;
        endif;
    }
}

/**
 * implements whats_on_cineworld_times function
 * @param $cinema -- cinema ID
 * @param $dates -- array of dates
 * @param $films -- array of movies showing in this cinema
 * @return mixed
 *
 */
function whats_on_cineworld_times($cinema , $dates , $films) {
    $movie = array();
    foreach ($films as $name => $film) {
        $movie[$film] =$dates;
    }
    foreach ($cinema as $cine) {
        $times[$cine] = $movie;
    }
    return $times;
}

/**
 * @return array
 * implements whats_on_cinema_id function -- returns array of cinema ids
 */
function whats_on_cinema_id() {
    $cinema_ids = db_select('node' , 'n')->fields('n', array('nid'))->condition('type' , 'cinema' , '=')->execute()->fetchAll();
    $cinema_id = array();
    foreach ($cinema_ids as $key => $nid) {
        $cinema_id[] = $nid->nid;
    }
    return $cinema_id;
}

/**
 * @param $nid
 * @return array
 * implements whats_on_nid_to_mid function
 * returns film id and release date
 */

function whats_on_nid_to_mid($nid){
    $node = node_load($nid);
    $film_id = array(
        'film_id' => $node->field_film_cineworld_id[LANGUAGE_NONE][0]['value'] ,
        'release_date' => $node->field_film_release_date[LANGUAGE_NONE][0]['value']
    );
    return $film_id;
}

/**
 * @param $nid
 * @return mixed
 * implements whats_on_nid_to_cid nid to cinema id
 */

function whats_on_nid_to_cid($nid){
    $node = node_load($nid);
    $cinema_id = $node->field_cinema_id[LANGUAGE_NONE][0]['value'];
    return $cinema_id;
}