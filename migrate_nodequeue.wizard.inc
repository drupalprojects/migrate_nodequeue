<?php

/**
 * @file
 * Wizard implementation for nodequeue migrations.
 */

// TODO ability to re-use node migration for mappings.
// TODO ability to re-use role migration for mappings.
// TODO should work only with D6 sources.
// TODO does it work when nodequeue not enabled on d6 ?
// TODO import variables ?
class MigrateNodequeueWizard extends MigrateD2DWizard {

  /**
   * Array of queues we will migrate.
   */
  protected $migratedQueues;

  /**
   * Whether we migrate queues content (nodes).
   */
  protected $migrateNodes;

  /**
   * Lay out the steps of the wizard.
   */
  public function __construct() {
    $this->addStep(t('Credentials'), 'sourceDataForm');
    $this->addStep(t('Queues'), 'nodequeueQueues');
    $this->addStep(t('Review'), 'reviewForm');
  }

  /**
   * Identify ourselves.
   *
   * @return string
   */
  public function getSourceName() {
    return t('Nodequeue');
  }

  /**
   * Present information and options around Queues migrations.
   */
  protected function nodequeueQueues(&$form_state) {
    //$form['#tree'] = TRUE; // TODO remove
    // TODO description 'select queues you want to migrate'
    $queues = $this->connection()
      ->select('nodequeue_queue', 'q')
      ->fields('q', array('qid', 'title'))
      ->execute()
      ->fetchAllKeyed(0);
    foreach ($queues as $qid => $title) {
      $nodes = $this->connection() // TODO use a subquery ?
        ->select('nodequeue_nodes', 'n')
        ->condition('qid', $qid)
        ->countQuery()
        ->execute()
        ->fetchField();
      $subqueues = $this->connection() // TODO use a subquery ?
        ->select('nodequeue_subqueue', 's')
        ->condition('qid', $qid)
        ->countQuery()
        ->execute()
        ->fetchField();
      $queues[$qid] = array(
        'title' => $title,
        'subqueues' => $subqueues,
        'nodes' => $nodes,
      );
    }
    $form['queues'] = array(
      '#type' => 'tableselect',
      '#header' => array('title' => t('Queue Title'), 'subqueues' => t('Number of Subqueues'), 'nodes' => t('Number of Nodes')),
      '#options' => $queues,
      '#empty' => t('No queue found on your Drupal!version site.', array('!version' => $this->sourceVersion)),
    );
    $form['nodes'] = array(
      '#type' => 'checkbox',
      '#title' => t('Also migrate queues content (nodes)'),
      '#default_value' => TRUE,
    );
    // TODO disable that if count(queues) is empty
    // TODO set default value if migration extends DrupalUserMigration
    // TODO description: 'you can create one with migrate_d2d_ui'
    /*
    $form['map_users'] = array(
      '#type' => 'select',
      '#title' => t('I have already migrated users'),
      '#options' => array(0 => t('- No -')) + array_keys(migrate_migrations()),
    );
    $form['map_roles'] = array(
      '#type' => 'select',
      '#title' => t('I have already migrated roles'),
      '#options' => array(0 => t('- No -')) + array_keys(migrate_migrations()),
    );
    */
    // TODO checkbox + ability to map content types
    return $form;
  }

  /**
   * Prepare for registration.
   *
   * @param array $form_state
   */
  protected function nodequeueQueuesValidate(&$form_state) {
    $this->migratedQueues = $form_state['values']['queues'];
    $this->migrateNodes   = $form_state['values']['nodes'];
  }

  /**
   * Show the user what they've chosen to migrate, and give them one last chance
   * to say yay or nay.
   */
  protected function reviewForm(&$form_state) {
    $this->groupArguments = array(
      'encrypted_arguments' => array('source_database'),
      'source_version' => $this->sourceVersion,
      'source_connection' => $this->groupName . '_legacy',
      'source_database' => $this->database,
      'source_system' => t('Drupal @version Nodequeues', array('@version' => $this->sourceVersion)),
    );

    $form['description'] = array(
      '#prefix' => '<div>',
      '#markup' => t('Please review your migration settings. When you submit this form, a migration job containing a migration task for each type of item to be imported will be created and you will be left at the dashboard.'),
      '#suffix' => '</div>',
    );

    // TODO do not do that if module disabled in d6
    // TODO make sure $arguments['descrition'] is set in the mgiratyion class
//    $this->addMigration('Queues', "NodequeueQueueDrupal{$this->sourceVersion}Migration", array());

    $imports['queues'] = array(
      '#theme' => 'item_list',
      '#prefix' => t('Queues to be migrated:'),
      '#items' => $this->connection()
        ->select('nodequeue_queue', 'q')
        ->fields('q', array('title'))
        ->condition('qid', $this->migratedQueues, 'IN')
        ->execute()
        ->fetchCol(),
    );

    if ($this->migrateNodes) {
      $this->addMigration('QueuesContent', "NodequeueNodesDrupal{$this->sourceVersion}Migration", array());
      $imports['nodes'] = array(
        '#prefix' => '<div>',
        '#markup' => '<p>' . t('Queues content (nodes) will be migrated.') . '</p>',
        '#suffix' => '</div>',
      );
    }

    $form['imports'] = array(
      '#theme' => 'item_list',
      '#items' => array_map('drupal_render', $imports),
    );

    return $form;
  }

}

