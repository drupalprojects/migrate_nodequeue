<?php

/**
 * @file
 * Wizard implementation for nodequeue migrations.
 */

// TODO ability to re-use node migration for mappings.
// TODO ability to re-use role migration for mappings.
// TODO ability to re-use type migration for mappings.
// TODO should work only with D6 sources.
// TODO does it work when nodequeue not enabled on d6 ?
// TODO import variables ?
class MigrateNodequeueWizard extends MigrateD2DWizard {

  /**
   * Array of queues we will migrate.
   */
  protected $source_qids;

  /**
   * Whether we migrate queues content (nodes).
   */
  protected $migrate_nodes;

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
    // TODO description 'select queues you want to migrate'
    $qids = $this->connection()
      ->select('nodequeue_queue', 'q')
      ->fields('q', array('qid', 'title'))
      ->orderBy('title')
      ->execute()
      ->fetchAllKeyed(0);
    foreach ($qids as $qid => $title) {
      $nodes = $this->connection()
        ->select('nodequeue_nodes', 'n')
        ->condition('qid', $qid)
        ->countQuery()
        ->execute()
        ->fetchField();
      $subqueues = $this->connection()
        ->select('nodequeue_subqueue', 's')
        ->condition('qid', $qid)
        ->countQuery()
        ->execute()
        ->fetchField();
      $qids[$qid] = array(
        'title' => $title,
        'subqueues' => $subqueues,
        'nodes' => $nodes,
      );
    }
    $form['qids'] = array(
      '#type' => 'tableselect',
      '#header' => array('title' => t('Queue Title'), 'subqueues' => t('Number of Subqueues'), 'nodes' => t('Number of Nodes')),
      '#options' => $qids,
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
    $this->source_qids   = $form_state['values']['qids'];
    $this->migrate_nodes = $form_state['values']['nodes'];
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
    $args = array(
      'description' => 'Migration of nodequeues',
      'source_qids' => $this->source_qids,
    );
    $this->addMigration('Nodequeues', "NodequeueQueueDrupal{$this->sourceVersion}Migration", $args);

    $imports['queues'] = array(
      '#theme' => 'item_list',
      '#prefix' => t('Queues to be migrated:'),
      '#items' => $this->connection()
        ->select('nodequeue_queue', 'q')
        ->fields('q', array('title'))
        ->condition('qid', $this->source_qids, 'IN')
        ->orderBy('title')
        ->execute()
        ->fetchCol(),
    );

    if ($this->migrate_nodes) {
      // TODO make sure $arguments['descrition'] is set in the mgiratyion class
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

